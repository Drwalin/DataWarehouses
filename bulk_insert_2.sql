
USE baza_webinarow
GO

BULK INSERT Uzytkownik_Systemu FROM '/run/media/drwalin/B8E096FFE096C354/studies/sem5/hd/lab/DataWarehouses/snapshots/2/users.csv' WITH (FIELDTERMINATOR=',', ROWTERMINATOR='\n');
GO

BULK INSERT Kurs FROM '/run/media/drwalin/B8E096FFE096C354/studies/sem5/hd/lab/DataWarehouses/snapshots/2/courses.csv' WITH (FIELDTERMINATOR=',', ROWTERMINATOR='\n');
GO

BULK INSERT Webinar FROM '/run/media/drwalin/B8E096FFE096C354/studies/sem5/hd/lab/DataWarehouses/snapshots/2/webinars.csv' WITH (FIELDTERMINATOR=',', ROWTERMINATOR='\n');
GO

BULK INSERT Pobyt_na_webinarze FROM '/run/media/drwalin/B8E096FFE096C354/studies/sem5/hd/lab/DataWarehouses/snapshots/2/stays.csv' WITH (FIELDTERMINATOR=',', ROWTERMINATOR='\n');
GO

BULK INSERT Problemy_Techniczne FROM '/run/media/drwalin/B8E096FFE096C354/studies/sem5/hd/lab/DataWarehouses/snapshots/2/problems.csv' WITH (FIELDTERMINATOR=',', ROWTERMINATOR='\n');
GO

