DROP TABLE IF EXISTS projet CASCADE;
DROP TABLE IF EXISTS soutient CASCADE;
DROP TABLE IF EXISTS test CASCADE;

CREATE TABLE projet(
	pid serial primary key,
	titre varchar(50),
	statut varchar(15),
	requis int
);

CREATE TABLE soutient(
	uid int,
	pid int references projet(pid),
	montant int
);

CREATE TABLE test(
	id serial primary key,
	a int,
	b int 
);

INSERT INTO projet VALUES
	(1,'Hoverboard','attente',50000),
	(2,'Full body VR','attente',10000),
	(3,'Perpetual motion','attente',500);

INSERT INTO soutient VALUES
	(2,2,6000);

--ERREUR:  erreur de syntaxe sur ou près de « INO »
--LIGNE 1 : INSERT INO test VALUES (1,6,7);
               
--rayane.dziri_db=!> SELECT * FROM test;
--ERREUR:  la transaction est annulée, les commandes sont ignorées jusqu'à la fin du bloc
--de la transaction
--rayane.dziri_db=!> COMMIT;
--ROLLBACK

-----------------------------------------------------------------------------
--exo2 
-- le niveau d'isolation est : read commited ;

