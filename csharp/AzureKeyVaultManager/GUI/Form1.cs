using Azure.Identity;
using Azure.Security.KeyVault.Secrets;
using Newtonsoft.Json;
using System.Windows.Forms;

namespace GUI
{
    public partial class Form1 : Form
    {
        private ClientSecretCredential clientSecretCredential;
        private SecretClient secretClient;
        private KeyVaultCredential keyVaultCredential;
        private const string KeyVaultCredentialFileName = "./KeyVaultCredential.json";

        public Form1()
        {
            InitializeComponent();

            dataGridView1.Columns.Clear();
            dataGridView1.Columns.Add("Name", "Name");
            dataGridView1.Columns.Add("Content-Type", "Visible");
            dataGridView1.Columns.Add("Has Tags", "Value");
        }

        private void keyVaultSettingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            var keyVaultSettingsDialog = new KeyVaultSettingsForm();

            if (File.Exists(KeyVaultCredentialFileName))
            {
                keyVaultCredential = JsonConvert.DeserializeObject<KeyVaultCredential>(
                    File.ReadAllText(KeyVaultCredentialFileName));
                keyVaultSettingsDialog.Controls["keyVaultUrlTextBox"].Text = keyVaultCredential.KeyVaultUrl;
                keyVaultSettingsDialog.Controls["tenantIdTextBox"].Text = keyVaultCredential.TenantId;
                keyVaultSettingsDialog.Controls["clientIdTextBox"].Text = keyVaultCredential.ClientId;
                keyVaultSettingsDialog.Controls["secretTextBox"].Text = keyVaultCredential.ClientSecret;
            }
            else
            {
                keyVaultCredential = new KeyVaultCredential();
            }

            var dialogResult = keyVaultSettingsDialog.ShowDialog();

            if (dialogResult == DialogResult.OK)
            {
                keyVaultCredential.KeyVaultUrl = keyVaultSettingsDialog.Controls["keyVaultUrlTextBox"].Text;
                keyVaultCredential.TenantId = keyVaultSettingsDialog.Controls["tenantIdTextBox"].Text;
                keyVaultCredential.ClientId = keyVaultSettingsDialog.Controls["clientIdTextBox"].Text;
                keyVaultCredential.ClientSecret = keyVaultSettingsDialog.Controls["secretTextBox"].Text;


                clientSecretCredential = new ClientSecretCredential(
                    keyVaultCredential.TenantId, keyVaultCredential.ClientId, keyVaultCredential.ClientSecret);
                secretClient = new SecretClient(
                    vaultUri: new Uri(keyVaultCredential.KeyVaultUrl), credential: clientSecretCredential);

                dataGridView1.Rows.Clear();

                foreach (var queryResponse in secretClient.GetPropertiesOfSecrets().AsPages().ToList())
                {
                    foreach (var entry in queryResponse.Values)
                    {
                        dataGridView1.Rows.Add(
                            entry.Name,
                            entry.ContentType,
                            entry.Tags.Count != 0);
                    }

                }

                dataGridView1.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.AllCells;

                File.WriteAllText(
                    KeyVaultCredentialFileName,
                    JsonConvert.SerializeObject(keyVaultCredential));
            }
        }
    }

    public class KeyVaultCredential
    {
        public string KeyVaultUrl;
        public string TenantId;
        public string ClientId;
        public string ClientSecret;
    }
}