/*
 * main.c — Minimal ESP32 + microSD (Catalex / generic SPI module) sample.
 *
 * Mounts a FAT filesystem from the SD card over SPI, writes a test file,
 * reads it back, lists the root directory, then unmounts.
 *
 * Wiring matches the original Boss GX-10 project so it can be used as a
 * standalone test rig for the same physical SD card module.
 *
 *   SD module pin   ESP32 GPIO   Notes
 *   -------------   ----------   --------------------------------------------
 *   CS              GPIO 16
 *   SCK             GPIO 15
 *   MOSI            GPIO 13
 *   MISO            GPIO 12      strapping pin — must be LOW at boot.
 *                                The Catalex module already has a pull-up
 *                                on MISO; if boot fails, hold MISO low
 *                                during reset or move to a different GPIO.
 *   VCC             5V (or 3V3 if your module is 3V3-only)
 *   GND             GND
 *
 * Notes about why the original code may have failed on some readers:
 *   - Some cheap SD modules / SD cards require a slow first SPI clock
 *     (≤ 400 kHz during init). ESP-IDF handles this automatically, but
 *     long, unshielded jumper wires can cause the negotiation to fail at
 *     the default 20 MHz transfer speed. Here we cap transfer speed at
 *     4 MHz (matches the original) and you can lower it via the
 *     SD_SPI_FREQ_KHZ define below if needed.
 *   - Internal pull-ups are enabled on MISO and CS to tolerate modules
 *     that lack pull-up resistors of their own.
 *   - The SPI bus is freed on every error path so re-running mount works.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "driver/gpio.h"

static const char *TAG = "SDCARD_SAMPLE";

/* ─── Pin configuration ──────────────────────────────────────────────── */
#define SD_PIN_MOSI         13
#define SD_PIN_MISO         12
#define SD_PIN_SCLK         15
#define SD_PIN_CS           16

#define SD_SPI_HOST         SPI2_HOST
#define SD_SPI_FREQ_KHZ     400         /* 400 kHz — slowest/safest. Bump to 1000/4000/20000 once it works. */

#define SD_MOUNT_POINT      "/sdcard"
#define SD_TEST_FILE        SD_MOUNT_POINT "/hello.txt"

/* Set to 1 to ERASE THE CARD and reformat it as FAT before the test runs.
 * THIS DESTROYS ALL DATA ON THE CARD. Set back to 0 after one successful run. */
#define SD_FORMAT_CARD      0

static sdmmc_card_t *s_card = NULL;

/* ──────────────────────────────────────────────────────────────────── */

static esp_err_t sdcard_mount(void)
{
    esp_err_t ret;

    /* Enable internal pull-ups on MISO and CS for modules without them. */
    gpio_set_pull_mode(SD_PIN_MISO, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(SD_PIN_CS,   GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(SD_PIN_MOSI, GPIO_PULLUP_ONLY);

    spi_bus_config_t bus_cfg = {
        .mosi_io_num     = SD_PIN_MOSI,
        .miso_io_num     = SD_PIN_MISO,
        .sclk_io_num     = SD_PIN_SCLK,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1,
        .max_transfer_sz = 4096,
    };

    ret = spi_bus_initialize(SD_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "spi_bus_initialize failed: %s", esp_err_to_name(ret));
        return ret;
    }

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot           = SD_SPI_HOST;
    host.max_freq_khz   = SD_SPI_FREQ_KHZ;

    sdspi_device_config_t slot_cfg = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_cfg.gpio_cs    = SD_PIN_CS;
    slot_cfg.host_id    = SD_SPI_HOST;

    esp_vfs_fat_sdmmc_mount_config_t mount_cfg = {
        .format_if_mount_failed = true,
        .max_files              = 5,
        .allocation_unit_size   = 16 * 1024,
    };

    ESP_LOGI(TAG, "Mounting SD card at %s (MOSI=%d MISO=%d SCLK=%d CS=%d, %d kHz)",
             SD_MOUNT_POINT, SD_PIN_MOSI, SD_PIN_MISO, SD_PIN_SCLK, SD_PIN_CS,
             SD_SPI_FREQ_KHZ);

    ret = esp_vfs_fat_sdspi_mount(SD_MOUNT_POINT, &host, &slot_cfg,
                                  &mount_cfg, &s_card);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Mount failed (ESP_FAIL) — could not mount the FAT filesystem.");
            ESP_LOGE(TAG, "  Possible causes:");
            ESP_LOGE(TAG, "    a. Unformatted card or non-FAT filesystem (format_if_mount_failed");
            ESP_LOGE(TAG, "       is now true, so this should auto-recover next boot).");
            ESP_LOGE(TAG, "    b. Sector read returned bad CRC (look for 0x108 above) — flaky");
            ESP_LOGE(TAG, "       contacts on the SD slot/adapter. Re-seat the card and retry.");
        } else if (ret == ESP_ERR_TIMEOUT) {
            ESP_LOGE(TAG, "Mount failed: ESP_ERR_TIMEOUT — the card never replied to SPI init.");
            ESP_LOGE(TAG, "  This is NOT a filesystem/format issue (the card never identified).");
            ESP_LOGE(TAG, "  Most likely causes, in order:");
            ESP_LOGE(TAG, "    1. Wiring: MOSI<->MISO swapped, loose jumper, bad solder joint.");
            ESP_LOGE(TAG, "    2. Power: brown-out during SD inrush — try a separate 3V3/5V supply");
            ESP_LOGE(TAG, "       to the module's VCC, with common GND, and a 10 uF cap at the module.");
            ESP_LOGE(TAG, "    3. Card not seated, or dead card — try another card.");
            ESP_LOGE(TAG, "    4. Module incompatibility — some Catalex 5V-only modules misbehave");
            ESP_LOGE(TAG, "       on 3V3 logic; a 3V3-native module is more reliable.");
        } else {
            ESP_LOGE(TAG, "Mount failed: %s. Check wiring and card insertion.",
                     esp_err_to_name(ret));
        }
        spi_bus_free(SD_SPI_HOST);
        return ret;
    }

    sdmmc_card_print_info(stdout, s_card);
    ESP_LOGI(TAG, "SD card mounted.");
    return ESP_OK;
}

static esp_err_t sdcard_unmount(void)
{
    if (!s_card) {
        return ESP_OK;
    }
    esp_err_t ret = esp_vfs_fat_sdcard_unmount(SD_MOUNT_POINT, s_card);
    s_card = NULL;
    spi_bus_free(SD_SPI_HOST);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Unmount failed: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "SD card unmounted.");
    }
    return ret;
}

static void sdcard_write_test_file(void)
{
    ESP_LOGI(TAG, "Writing %s …", SD_TEST_FILE);
    FILE *f = fopen(SD_TEST_FILE, "w");
    if (!f) {
        ESP_LOGE(TAG, "fopen(\"%s\", \"w\") failed: errno=%d (%s)",
                 errno, strerror(errno));
        return;
    }
    int n1 = fprintf(f, "Hello from ESP32 SD sample!\n");
    int n2 = fprintf(f, "Tick count at write: %lu\n",
                     (unsigned long)xTaskGetTickCount());
    if (n1 < 0 || n2 < 0) {
        ESP_LOGE(TAG, "fprintf failed: errno=%d (%s)", errno, strerror(errno));
    }
    if (fflush(f) != 0) {
        ESP_LOGE(TAG, "fflush failed: errno=%d (%s)", errno, strerror(errno));
    }
    if (fsync(fileno(f)) != 0) {
        ESP_LOGE(TAG, "fsync failed: errno=%d (%s)", errno, strerror(errno));
    }
    if (fclose(f) != 0) {
        ESP_LOGE(TAG, "fclose failed: errno=%d (%s)", errno, strerror(errno));
        return;
    }
    ESP_LOGI(TAG, "Write OK (%d bytes).", n1 + n2);
}

static void sdcard_read_test_file(void)
{
    ESP_LOGI(TAG, "Reading %s …", SD_TEST_FILE);
    FILE *f = fopen(SD_TEST_FILE, "r");
    if (!f) {
        ESP_LOGE(TAG, "fopen(\"%s\", \"r\") failed", SD_TEST_FILE);
        return;
    }
    char line[128];
    while (fgets(line, sizeof(line), f)) {
        size_t n = strlen(line);
        if (n && line[n - 1] == '\n') {
            line[n - 1] = '\0';
        }
        ESP_LOGI(TAG, "  %s", line);
    }
    fclose(f);
}

static void sdcard_list_root(void)
{
    ESP_LOGI(TAG, "Listing %s", SD_MOUNT_POINT);
    DIR *d = opendir(SD_MOUNT_POINT);
    if (!d) {
        ESP_LOGE(TAG, "opendir failed");
        return;
    }
    struct dirent *de;
    while ((de = readdir(d)) != NULL) {
        char full[300];
        snprintf(full, sizeof(full), "%s/%s", SD_MOUNT_POINT, de->d_name);
        struct stat st;
        if (stat(full, &st) == 0) {
            ESP_LOGI(TAG, "  %-32s %ld bytes%s",
                     de->d_name, (long)st.st_size,
                     S_ISDIR(st.st_mode) ? " (dir)" : "");
        } else {
            ESP_LOGI(TAG, "  %s", de->d_name);
        }
    }
    closedir(d);
}

void app_main(void)
{
    ESP_LOGI(TAG, "ESP32 SD card SPI sample starting");

    if (sdcard_mount() != ESP_OK) {
        ESP_LOGE(TAG, "Aborting: SD mount failed.");
        return;
    }

#if SD_FORMAT_CARD
    ESP_LOGW(TAG, "SD_FORMAT_CARD=1 — ERASING AND REFORMATTING THE CARD.");
    esp_err_t fret = esp_vfs_fat_sdcard_format(SD_MOUNT_POINT, s_card);
    if (fret != ESP_OK) {
        ESP_LOGE(TAG, "Format failed: %s", esp_err_to_name(fret));
    } else {
        ESP_LOGI(TAG, "Format OK. Set SD_FORMAT_CARD back to 0 and re-flash.");
    }
#endif

    sdcard_write_test_file();
    sdcard_read_test_file();
    sdcard_list_root();

    sdcard_unmount();

    ESP_LOGI(TAG, "Done. Idling.");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
