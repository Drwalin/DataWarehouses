
DROP DATABASE IF EXISTS baza_webinarow;
GO

CREATE DATABASE baza_webinarow
GO

USE baza_webinarow
GO

CREATE TABLE Uzytkownik_Systemu (
	id CHAR(16) PRIMARY KEY,
	nazwaUzytkownika NVARCHAR(MAX) NOT NULL,
	rola NVARCHAR(MAX) NOT NULL
);
GO

CREATE TABLE Kurs (
	id CHAR(16) PRIMARY KEY,
	nazwaPrzedmiotu NVARCHAR(MAX) NOT NULL,
	ECTS INT,
	semestrStudiow INT
);
GO

CREATE TABLE Webinar (
	id CHAR(16) PRIMARY KEY,
	id_kurs CHAR(16) NOT NULL,
	CONSTRAINT fk_id_kurs FOREIGN KEY (id_kurs)
		REFERENCES Kurs (id) ON DELETE CASCADE,
	dataRozpoczecia DATETIME,
	dataZakonczenia DATETIME,
	planowanaDataRozpoczecia DATETIME NOT NULL,
	planowanaDataZakonczenia DATETIME NOT NULL,
	wystapilyProblemyTechniczne BIT
);
GO

CREATE TABLE Pobyt_na_webinarze (
	id CHAR(16) PRIMARY KEY,
	id_uzytkownik CHAR(16) NOT NULL,
	CONSTRAINT fk_id_uzytkownik FOREIGN KEY (id_uzytkownik)
		REFERENCES Uzytkownik_Systemu (id) ON DELETE CASCADE,
	dataWejscia DATETIME NOT NULL,
	dataWyjscia DATETIME NOT NULL,
	id_webinar CHAR(16) NOT NULL,
	CONSTRAINT fk_id_webinar_2 FOREIGN KEY (id_webinar)
		REFERENCES Webinar (id) ON DELETE CASCADE,
);
GO

CREATE TABLE Problemy_Techniczne (
	id CHAR(16) PRIMARY KEY,
	id_webinar CHAR(16) NOT NULL,
	CONSTRAINT fk_id_webinar FOREIGN KEY (id_webinar)
		REFERENCES Webinar (id) ON DELETE CASCADE,
	czasTrwania INT,
	rodzaj NVARCHAR(MAX),
	winowajca NVARCHAR(MAX),
);
GO






