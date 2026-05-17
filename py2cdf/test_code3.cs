import ooc.h
import db_sqlite.h
import io.h

def db():
    db = SQLiteDatabase("test.db")
    console = Console();
    console.print_object(db);
    conn = db.open();
    sql = "DROP TABLE IF EXISTS Cars;CREATE TABLE Cars(Id INT, Name TEXT, Price REAL);INSERT INTO Cars VALUES(1, 'Audi', 52642.99);"
			
    conn.exec(sql)
    sql2 = "SELECT * FROM Cars;"
    results = conn.query(sql2)
    console.print_object(results)




