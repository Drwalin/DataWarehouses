
USE baza_webinarow
GO

CREATE TABLE #temp (id CHAR(16), b NVARCHAR(MAX), c NVARCHAR(MAX));
BULK INSERT #temp FROM '/run/media/drwalin/B8E096FFE096C354/studies/sem5/hd/lab/DataWarehouses/snapshots/2/users.csv' WITH (FIELDTERMINATOR=',', ROWTERMINATOR='\n');
INSERT INTO Uzytkownik_Systemu SELECT * FROM #temp WHERE NOT EXISTS
(SELECT id from #temp WHERE #temp.id = Uzytkownik_Systemu.id);

DROP TABLE #temp
GO

CREATE TABLE #temp (id CHAR(16), b NVARCHAR(MAX), c INT, d INT);
BULK INSERT #temp FROM '/run/media/drwalin/B8E096FFE096C354/studies/sem5/hd/lab/DataWarehouses/snapshots/2/courses.csv' WITH (FIELDTERMINATOR=',', ROWTERMINATOR='\n');
INSERT INTO Kurs SELECT * FROM #temp WHERE NOT EXISTS
(SELECT id from #temp WHERE #temp.id = Kurs.id);

DROP TABLE #temp
GO

CREATE TABLE #temp (id CHAR(16), b CHAR(16), c DATETIME, d DATETIME, e DATETIME, f DATETIME, g BIT);
BULK INSERT #temp FROM '/run/media/drwalin/B8E096FFE096C354/studies/sem5/hd/lab/DataWarehouses/snapshots/2/webinars.csv' WITH (FIELDTERMINATOR=',', ROWTERMINATOR='\n');
INSERT INTO Webinar SELECT * FROM #temp WHERE NOT EXISTS
(SELECT id from #temp WHERE #temp.id = Webinar.id);

DROP TABLE #temp
GO

CREATE TABLE #temp (id CHAR(16), b, c, d, e);
BULK INSERT #temp FROM '/run/media/drwalin/B8E096FFE096C354/studies/sem5/hd/lab/DataWarehouses/snapshots/2/stays.csv' WITH (FIELDTERMINATOR=',', ROWTERMINATOR='\n');
INSERT INTO Pobyt_na_webinarze SELECT * FROM #temp WHERE NOT EXISTS
(SELECT id from #temp WHERE #temp.id = Pobyt_na_webinarze.id);

DROP TABLE #temp
GO

CREATE TABLE #temp (id CHAR(16), b, c, d, e);
BULK INSERT $temp5 FROM '/run/media/drwalin/B8E096FFE096C354/studies/sem5/hd/lab/DataWarehouses/snapshots/2/problems.csv' WITH (FIELDTERMINATOR=',', ROWTERMINATOR='\n');
INSERT INTO Kurs SELECT * FROM #temp WHERE NOT EXISTS
(SELECT id from #temp WHERE #temp.id = Kurs.id);
Problemy_Techniczne

DROP TABLE #temp
GO

