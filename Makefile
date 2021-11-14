
run:
	g++ main.cpp -pedantic -Wall -O4
	./a.out

sql-cli:
	sqlcmd -S localhost -U sa

sql-file:
	sqlcmd -S localhost -U sa -i $(file)

