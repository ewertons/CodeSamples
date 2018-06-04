using System;
using System.Data.SqlClient;

namespace SqlConnectionSample
{
    class Program
    {
        static void Main(string[] args)
        {
            string connectionString = "Server=...;Connection Timeout=30;";
            int result;

            SqlConnection sqlConnection = new SqlConnection(connectionString);
            sqlConnection.Open();

            // Be careful with reserved words. Do not use them for table fields.
            // https://docs.microsoft.com/en-us/sql/t-sql/language-elements/reserved-keywords-transact-sql?view=sql-server-2017

            SqlCommand createTableCmd = new SqlCommand("CREATE TABLE TestTable (idx int, itemname varchar(255), itemdate datetime, itemvalue float);", sqlConnection);

            result = createTableCmd.ExecuteNonQuery();

            for (int i = 1; i< 10; i++)
            {
                SqlCommand insertCmd = new SqlCommand($"INSERT INTO TestTable VALUES ({i}, 'abcde{i}', {i}/10/1981, 1{i}.1)", sqlConnection);
                result = insertCmd.ExecuteNonQuery();
            }

            SqlCommand query = new SqlCommand("SELECT * FROM TestTable WHERE idx > 3 AND idx < 7", sqlConnection);

            SqlDataReader dataReader = query.ExecuteReader();

            while (dataReader.Read())
            {
                Console.WriteLine($"{dataReader[0]}, {dataReader[1]}, {dataReader[3]}, {dataReader[3]}");
            }

            dataReader.Close();

            SqlCommand deleteCmd = new SqlCommand("DROP TABLE TestTable", sqlConnection);
            result = deleteCmd.ExecuteNonQuery();

            sqlConnection.Close();
        }
    }
}
