// http://zetcode.com/db/mysqlc/

#include <my_global.h>
#include <mysql.h>

#define DB_USER "root"
#define DB_PASSWORD "SetYourDBUserPassword"

// sudo apt-get install libmysqlclient-dev

static void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

static void create_database()
{
  MYSQL *con = mysql_init(NULL);

  if (con == NULL) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      return;
  }

  if (mysql_real_connect(con, "localhost", DB_USER, DB_PASSWORD, NULL, 0, NULL, 0) == NULL) 
  {
      finish_with_error(con);
  }  

  if (mysql_query(con, "CREATE DATABASE testdb")) 
  {
      finish_with_error(con);
  }

  // To list databases:
  // SHOW databases;

  mysql_close(con);
}

static void create_table_and_insert()
{
  MYSQL *con = mysql_init(NULL);
  
  if (con == NULL) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }  

  if (mysql_real_connect(con, "localhost", DB_USER, DB_PASSWORD, "testdb", 0, NULL, 0) == NULL) 
  {
      finish_with_error(con);
  }    
  
  if (mysql_query(con, "DROP TABLE IF EXISTS Cars")) {
      finish_with_error(con);
  }
  
  if (mysql_query(con, "CREATE TABLE Cars(Id INT, Name TEXT, Price INT)")) {      
      finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(1,'Audi',52642)")) {
      finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(2,'Mercedes',57127)")) {
      finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(3,'Skoda',9000)")) {
      finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(4,'Volvo',29000)")) {
      finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(5,'Bentley',350000)")) {
      finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(6,'Citroen',21000)")) {
      finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(7,'Hummer',41400)")) {
      finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO Cars VALUES(8,'Volkswagen',21600)")) {
      finish_with_error(con);
  }

  mysql_close(con);
}

static void query()
{      
  MYSQL *con = mysql_init(NULL);
  
  if (con == NULL)
  {
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);
  }  
  
  if (mysql_real_connect(con, "localhost", DB_USER, DB_PASSWORD, "testdb", 0, NULL, 0) == NULL) 
  {
      finish_with_error(con);
  }    
  
  if (mysql_query(con, "SELECT * FROM Cars")) 
  {
      finish_with_error(con);
  }
  
  MYSQL_RES *result = mysql_store_result(con);
  
  if (result == NULL) 
  {
      finish_with_error(con);
  }

  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;
  MYSQL_FIELD *field;
  
  while ((row = mysql_fetch_row(result))) 
  { 
      for(int i = 0; i < num_fields; i++) 
      {
          if (i == 0) 
          {              
             while(field = mysql_fetch_field(result)) 
             {
                printf("%s ", field->name);
             }
             
             printf("\n");           
          }

          printf("%s ", row[i] ? row[i] : "NULL"); 
      } 
      
      printf("\n"); 
  }
  
  mysql_free_result(result);
  mysql_close(con);
}

static void drop_database()
{
  MYSQL *con = mysql_init(NULL);

  if (con == NULL) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      return;
  }

  if (mysql_real_connect(con, "localhost", DB_USER, DB_PASSWORD, NULL, 0, NULL, 0) == NULL) 
  {
      finish_with_error(con);
  }  

  if (mysql_query(con, "DROP DATABASE testdb")) 
  {
      finish_with_error(con);
  }

  // To list databases:
  // SHOW databases;

  mysql_close(con);
}

int main(int argc, char **argv)
{
  printf("MySQL client version: %s\n", mysql_get_client_info());

  create_database();
  create_table_and_insert();
  query();
  drop_database();

  // see also:mysql_real_connect option CLIENT_MULTI_STATEMENTS

  exit(0);
}
