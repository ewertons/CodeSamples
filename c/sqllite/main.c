/*
 * SQLite C API sample.
 *
 * Demonstrates the common database operations using the SQLite C API:
 *   - opening/creating a database file (the "create DB" step)
 *   - creating a table
 *   - inserting rows (with prepared statements + bound parameters)
 *   - updating rows
 *   - deleting rows
 *   - searching/selecting rows
 *   - dropping a table
 *   - closing and removing the DB file (the "drop DB" step)
 *
 * Build with the provided CMakeLists.txt. Works on Linux and Windows.
 */

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_FILENAME "sample.db"

/* Helper: bail out on a SQLite error, printing a message and closing the DB. */
static int die(sqlite3 *db, const char *what)
{
    fprintf(stderr, "ERROR: %s: %s\n", what, sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
}

/* Execute a non-query SQL statement (CREATE/DROP/INSERT/UPDATE/DELETE without
 * parameters). Returns SQLITE_OK on success. */
static int exec_sql(sqlite3 *db, const char *sql)
{
    char *errmsg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error running [%s]: %s\n", sql, errmsg);
        sqlite3_free(errmsg);
    }
    return rc;
}

/* Print all rows of the users table, optionally filtering by a name pattern.
 * Pass NULL to dump everything. */
static int print_users(sqlite3 *db, const char *name_like)
{
    const char *sql_all   = "SELECT id, name, age FROM users ORDER BY id;";
    const char *sql_where = "SELECT id, name, age FROM users WHERE name LIKE ? ORDER BY id;";

    sqlite3_stmt *stmt = NULL;

    /* sqlite3_prepare_v2: compile a SQL string into a reusable "prepared
     * statement" (sqlite3_stmt). The '?' characters in the SQL are placeholders
     * for parameters that we'll bind below. Compiling once and reusing is
     * faster than re-parsing the SQL each time, and it prevents SQL injection.
     * Args: (db, sql, nbytes_or_-1_for_nul_terminated, out_stmt, out_tail). */
    int rc = sqlite3_prepare_v2(db, name_like ? sql_where : sql_all, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "prepare failed: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    if (name_like) {
        /* sqlite3_bind_text: substitute the 1st '?' placeholder with a string.
         * Parameter indexes are 1-based. The -1 means "string is NUL-terminated,
         * figure out the length". SQLITE_TRANSIENT tells SQLite to make its own
         * copy of the buffer (safe even if our buffer goes out of scope). Use
         * SQLITE_STATIC instead if the buffer is guaranteed to live long enough. */
        sqlite3_bind_text(stmt, 1, name_like, -1, SQLITE_TRANSIENT);
        printf("  (filter: name LIKE '%s')\n", name_like);
    }

    printf("  %-4s  %-20s  %s\n", "id", "name", "age");
    printf("  ----  --------------------  ---\n");

    int row_count = 0;
    /* sqlite3_step: advance the prepared statement.
     *   - returns SQLITE_ROW   when a result row is available (read columns)
     *   - returns SQLITE_DONE  when there are no more rows / statement finished
     *   - returns an error code otherwise
     * For non-SELECT statements (INSERT/UPDATE/DELETE) you typically step exactly
     * once and expect SQLITE_DONE. */
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        /* sqlite3_column_*: read column values from the current result row.
         * Column indexes are 0-based (unlike bind indexes which are 1-based). */
        int         id   = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        int         age  = sqlite3_column_int(stmt, 2);
        printf("  %-4d  %-20s  %d\n", id, name ? name : "(null)", age);
        row_count++;
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "step failed: %s\n", sqlite3_errmsg(db));
    }
    printf("  (%d row(s))\n", row_count);

    /* sqlite3_finalize: destroy the prepared statement and free its resources.
     * Must be called for every successful sqlite3_prepare_v2, otherwise the
     * statement leaks (and will keep locks on the DB). */
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

/* Insert one row using a prepared statement with bound parameters
 * (the safe way: no SQL injection, correct quoting/typing). */
static int insert_user(sqlite3 *db, const char *name, int age)
{
    /* Two '?' placeholders -> we'll bind two parameters (name, age). */
    const char *sql = "INSERT INTO users (name, age) VALUES (?, ?);";
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "prepare insert failed: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    /* Bind values to the placeholders by position (1-based).
     * There is one sqlite3_bind_* per SQL type:
     *   sqlite3_bind_int / _int64 / _double / _text / _blob / _null. */
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 2, age);

    /* For INSERT/UPDATE/DELETE, a single step runs the statement to completion. */
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "insert failed: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? SQLITE_OK : rc;
}

int main(void)
{
    printf("SQLite library version: %s\n\n", sqlite3_libversion());

    /* Start clean: remove any leftover DB file from a previous run so the
     * "create database" step is meaningful. */
    remove(DB_FILENAME);

    sqlite3 *db = NULL;

    /* ---- 1. Create / open the database file ---- */
    printf("[1] Opening (creating) database file '%s'...\n", DB_FILENAME);
    if (sqlite3_open(DB_FILENAME, &db) != SQLITE_OK) {
        return die(db, "sqlite3_open");
    }

    /* ---- 2. Create a table ---- */
    printf("[2] Creating table 'users'...\n");
    const char *create_sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "  id   INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT    NOT NULL,"
        "  age  INTEGER NOT NULL"
        ");";
    if (exec_sql(db, create_sql) != SQLITE_OK) {
        return die(db, "create table");
    }

    /* ---- 3. Insert rows ---- */
    printf("[3] Inserting rows...\n");
    /* Wrap multiple inserts in a transaction for speed and atomicity. */
    exec_sql(db, "BEGIN;");
    insert_user(db, "Alice",   30);
    insert_user(db, "Bob",     25);
    insert_user(db, "Charlie", 35);
    insert_user(db, "Dave",    40);
    exec_sql(db, "COMMIT;");
    print_users(db, NULL);

    /* ---- 4. Update a row ---- */
    printf("\n[4] Updating Bob's age to 26...\n");
    {
        const char *sql = "UPDATE users SET age = ? WHERE name = ?;";
        sqlite3_stmt *stmt = NULL;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            return die(db, "prepare update");
        }
        sqlite3_bind_int (stmt, 1, 26);
        sqlite3_bind_text(stmt, 2, "Bob", -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "update failed: %s\n", sqlite3_errmsg(db));
        }
        printf("  rows affected: %d\n", sqlite3_changes(db));
        sqlite3_finalize(stmt);
    }
    print_users(db, NULL);

    /* ---- 5. Search (SELECT with WHERE / LIKE) ---- */
    printf("\n[5] Searching for names starting with 'A' or 'C'...\n");
    print_users(db, "A%");
    print_users(db, "C%");

    /* ---- 6. Delete a row ---- */
    printf("\n[6] Deleting Dave...\n");
    {
        const char *sql = "DELETE FROM users WHERE name = ?;";
        sqlite3_stmt *stmt = NULL;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            return die(db, "prepare delete");
        }
        sqlite3_bind_text(stmt, 1, "Dave", -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "delete failed: %s\n", sqlite3_errmsg(db));
        }
        printf("  rows affected: %d\n", sqlite3_changes(db));
        sqlite3_finalize(stmt);
    }
    print_users(db, NULL);

    /* ---- 7. Drop the table ---- */
    printf("\n[7] Dropping table 'users'...\n");
    if (exec_sql(db, "DROP TABLE users;") != SQLITE_OK) {
        return die(db, "drop table");
    }

    /* ---- 8. Close and remove the DB file (the "drop database" equivalent
     * for a file-based engine like SQLite) ---- */
    printf("[8] Closing database and removing file...\n");
    sqlite3_close(db);
    if (remove(DB_FILENAME) != 0) {
        perror("remove");
    }

    printf("\nDone.\n");
    return 0;
}
