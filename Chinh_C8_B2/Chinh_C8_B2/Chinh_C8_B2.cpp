// Chinh_C8_B2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include "math.h"
#include "windows.h"
#include "time.h"
#include "CType.h"
#include "string.h"

const int MAXCAT = 10; // danh mục tối đa
const int MAXSONG = 100; // bài hát tối đa
const int MAXLINE = 256; // mỗi dòng tối đa 256 kí tự
const int MAX = 10;
char *username = 0;
char *password = 0;

struct Account
{
	char *username = new char [MAX];
	char *password = new char [MAX];
	int status = 1;
};

struct ListAccount
{
	Account *account = new Account[MAX];
	int n;
};

struct Date
{
	int day;
	int month;
	int year;
};

struct Category
{
	int id;
	char *name = new char[MAXLINE];
	int status = 1;
};

struct ListCategory
{
	Category *categories = new Category[MAXCAT];
	int n = 0;
};

struct SONG
{
	int id;
	int idCat; // mã thể loại
	Category category; // loading related 
	char *name = new char[256];
	Date dateLoad;
	int status = 1;
};

struct ListSong
{
	SONG *songs = new SONG[MAXSONG];
	int n = 0;
};

struct Database
{
	char **database;
	int iLines = 0;
};

void(*loginPointer)(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName);
void(*adminPointer)(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName);
void(*userPointer)(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName);

void OpenFile(FILE *&f, char *fileName, char *mode)
{
	errno_t err = fopen_s(&f, fileName, mode);
	if (err != 0)
		printf("The file was not opened\n");
}

void Input(FILE *&f, Database &db, char *fileName, char *mode)
{
	OpenFile(f, fileName, mode);
	fscanf_s(f, "%d", &db.iLines);
	fgetc(f); // pass qua 1 kí tự \n
	db.database = new char*[db.iLines];

	int index = 0;
	while (!feof(f))
	{
		db.database[index] = new char[MAXLINE];
		fgets(db.database[index], MAXLINE, f);
		index++;
	}
	fclose(f);
}

int ChuyenDoiSo(char c)
{
	if (c >= 48 && c <= 57)
		return c - 48;
	return -1; // không chuyển đổi được
}

int ChuyenDoiChuoiSo(char *s)
{
	int length = strlen(s);
	int number = 0;
	for (int i = 0; i < strlen(s); i++)
	{
		number += ChuyenDoiSo(s[i]) * pow(10 * 1.0, (length - 1) * 1.0);
		length--;
	}
	return number;
}

char *TachTheoKiTu(char *str, char delim, char *&next)
{
	if (str != NULL)
		next = str;
	str = next;
	for (int i = 0; i < strlen(str); i++)
	{
		next++;
		if (str[i] == delim || i == strlen(str) - 1)
			str[i] = '\0';
	}

	if (strlen(str) == 0)
		return NULL;
	return str;
}

void Load(ListCategory &lstCategory, Database db)
{
	char *temp;
	char delim = ',';
	char *next;
	lstCategory.n = db.iLines;
	for (int i = 0; i < db.iLines; i++)
	{
		temp = TachTheoKiTu(db.database[i], delim, next);
		lstCategory.categories[i].id = ChuyenDoiChuoiSo(temp);
		while (temp != NULL)
		{
			temp = TachTheoKiTu(NULL, delim, next);
			lstCategory.categories[i].name = temp;

			temp = TachTheoKiTu(NULL, delim, next);
			lstCategory.categories[i].status = ChuyenDoiChuoiSo(temp);
			if (*next == '\n' || *next == '\0')
				break;
		}
	}
}

Category *getCategory(ListCategory lstCategory, int idCat)
{
	Category *p = lstCategory.categories;
	for (int i = 0; i < lstCategory.n; i++)
	{
		if (p->id == idCat)
			return p;
		p++;
	}
	return NULL;
}

void Load(ListCategory lstCategory, ListSong &lstSong, Database db)
{
	char *temp;
	char delim = ',';
	char *next;
	lstSong.n = db.iLines;
	for (int i = 0; i < db.iLines; i++)
	{
		// get id
		temp = TachTheoKiTu(db.database[i], delim, next);
		lstSong.songs[i].id = ChuyenDoiChuoiSo(temp);
		while (temp != NULL)
		{
			// get idCat
			temp = TachTheoKiTu(NULL, delim, next);
			lstSong.songs[i].idCat = ChuyenDoiChuoiSo(temp);
			// lazy loading or load related
			lstSong.songs[i].category = *getCategory(lstCategory, lstSong.songs[i].idCat);
			// get Name 
			temp = TachTheoKiTu(NULL, delim, next);
			lstSong.songs[i].name = temp;
			// get day, month, year
			temp = TachTheoKiTu(NULL, delim, next);
			lstSong.songs[i].dateLoad.day = ChuyenDoiChuoiSo(temp);
			temp = TachTheoKiTu(NULL, delim, next);
			lstSong.songs[i].dateLoad.month = ChuyenDoiChuoiSo(temp);
			temp = TachTheoKiTu(NULL, delim, next);
			lstSong.songs[i].dateLoad.year = ChuyenDoiChuoiSo(temp);
			temp = TachTheoKiTu(NULL, delim, next);
			lstSong.songs[i].status = ChuyenDoiChuoiSo(temp);

			if (*next == '\n' || *next == '\0')
				break;
		}
	}
}

// DRAW A TABLE LIST! / VẼ LIST!
void DrawSpace(int n)
{
	while (n > 0)
	{
		printf(" ");
		n--;
	}
}

void DrawTitle(char c[], int n, int m, int count)
{
	Draw(186);
	DrawSpace(n);
	if (count == 0)
		printf("%s", c);
	else
		printf("%s %d", c, count);
	DrawSpace(m);
}

void DrawTitle(int length, int flag)
{
	DrawTitle("STT", 3, 4, 0);
	DrawTitle("Name Genres", 4, 6, 0);
	if (flag == 1)
	{
		DrawTitle("Name Songs", 5, 5, 0);
		DrawTitle("Date Load", 5, 6, 0);
	}
	DrawTitle("Status", 6, 8, 0);
	
	Draw(186);
}

int CountIndex(int n)
{
	int count = 0;
	if (n == 0)
		return 1;

	while (n > 0)
	{
		n /= 10;
		count++;
	}
	return count;
}

void DrawSTT()
{
	printf("\n");
	Draw(204);
	for (int i = 0; i < 10; i++)
		Draw(205);
	Draw(206);
}

void DrawRow(int n, int m, int Number1, int Number2, int Number3, int Number4, int flag)
{
	int temp = n;

	if (flag == 1)
	{
		printf("\n");
		Draw(Number1);
		for (int i = 0; i < 10; i++)
			Draw(Number3);
		Draw(Number2);
		temp -= 21;
	}	
	n = temp;
	temp -= m;
	for (int idx = n; idx > 0; idx--)
	{
		if (idx == temp)
		{
			Draw(Number2);
			temp -= m;
		}
		else
			Draw(Number3);
	}
		Draw(Number4);
		printf("\n");
}

void Draw(int n, char c)
{
	for (int i = 1; i <= n; i++)
		printf("%c", c);
}

void Draw(int spaceV1, int spaceV2, int count, char c)
{
	for (int i = 1; i <= spaceV2 - (count + spaceV1); i++)
		printf("%c", c);
}

int isCharInt(int index)
{
	if (isUpper(index) || isLower(index))
		return 1;
	return 0;
}

void DrawChar(char *Index, int spaceV1, int spaceV2, int flag)
{
	if (flag == 1)
		Draw(186);
	Draw(spaceV1, ' ');
	printf("%s", Index);
	Draw(spaceV1, spaceV2, 1, ' ');
}

void DrawInt(int Inter, int spaceV1, int spaceV2, int flag)
{
	if (flag == 1)
		Draw(186);
	Draw(spaceV1, ' ');
	printf("%d", Inter);
	Draw(spaceV1, spaceV2, CountIndex(Inter), ' ');
}

void DrawDate(ListSong lstSong, int n, int spaceV1, int spaceV2)
{
	Draw(spaceV1, ' ');
	if (lstSong.songs[n].dateLoad.day < 10)
		printf("0%d/", lstSong.songs[n].dateLoad.day);
	else
	    printf("%d/", lstSong.songs[n].dateLoad.day);

	if (lstSong.songs[n].dateLoad.month < 10)
		printf("0%d/", lstSong.songs[n].dateLoad.month);
	else
		printf("%d/", lstSong.songs[n].dateLoad.month);
	printf("%d", lstSong.songs[n].dateLoad.year);
	Draw(spaceV1, spaceV2, 11, ' ');
}

void DrawSTT(int Value)
{
	int lengthChar = 10;
	DrawInt(Value, 4, lengthChar, 1);
	Draw(186);
}

void DrawNameGenres(char *Char)
{
	int lengthChar = 22 - getLength(Char);
	DrawChar(Char, 4, lengthChar, 0);
	Draw(186);
}

void DrawNameSongs(char *Char)
{
	int lengthChar = 21 - getLength(Char);
	DrawChar(Char, 1, lengthChar, 0);
	Draw(186);
}

void DrawNameDate(ListSong lstSong, int n)
{
	DrawDate(lstSong, n, 4, 21);
	Draw(186);
}

void DrawStatus(int Value, int values)
{
	char *string;
	if (Value == 1)
	{
		SET_COLOR(9);
		string = "Active";
	}
		
	else
	{
		SET_COLOR(12);
		string = "Lock";
	}
		
	int lengthChar = 21 - getLength(string);
	DrawChar(string, 6, lengthChar, 0);
	SET_COLOR(15);
	Draw(186);
}

void OutputTitle(int Value, int flag)
{
	int lengthBody = Value * 21;
	DrawRow(lengthBody, 21, 201, 203, 205, 187, 1);
	DrawTitle(lengthBody / 21, flag);
}

void Output(ListCategory lstCategory) 
{
	OutputTitle(3, 0);

	for (int i = 0; i < lstCategory.n; i++)
	{
		DrawSTT();
		DrawRow(42, 21, 204, 206, 205, 185, 0);
		DrawSTT(lstCategory.categories[i].id);
		DrawNameGenres( lstCategory.categories[i].name);
		DrawStatus(lstCategory.categories[i].status, 1);
	}
	DrawRow(63, 21, 200, 202, 205, 188, 1);
}

void Output(ListSong lstSong) 
{	
	OutputTitle(5, 1);
	for (int i = 0; i < lstSong.n; i++)
	{
		DrawSTT();
		DrawRow(84, 21, 204, 206, 205, 185, 0);
		DrawSTT(lstSong.songs[i].id);
		DrawNameGenres(lstSong.songs[i].category.name);
		DrawNameSongs(lstSong.songs[i].name);
		DrawNameDate(lstSong, i);
		DrawStatus(lstSong.songs[i].status, 1);
	}
	DrawRow(105, 21, 200, 202, 205, 188, 1);
}

void OutputPosCate(ListCategory lstCategory, char *cate)
{
	OutputTitle(3, 0);
	for (int i = 0; i < lstCategory.n; i++)
	{
		if (!_strcmpi(lstCategory.categories[i].name, cate))
		{
			DrawSTT();
			DrawRow(42, 21, 204, 206, 205, 185, 0);
			DrawSTT(lstCategory.categories[i].id);
			DrawNameGenres(lstCategory.categories[i].name);
			DrawStatus(lstCategory.categories[i].status, 1);
		}	
	}
	DrawRow(63, 21, 200, 202, 205, 188, 1);
}

void OutputPosSong(ListSong lstSong, char *song)
{
	OutputTitle(5, 1);
	for (int i = 0; i < lstSong.n; i++)
	{
		if (!_strcmpi(lstSong.songs[i].name, song))
		{
			DrawSTT();
			DrawRow(84, 21, 204, 206, 205, 185, 0);
			DrawSTT(lstSong.songs[i].id);
			DrawNameGenres(lstSong.songs[i].category.name);
			DrawNameSongs(lstSong.songs[i].name);
			DrawNameDate(lstSong, i);
			DrawStatus(lstSong.songs[i].status, 1);
		}
	}
	DrawRow(105, 21, 200, 202, 205, 188, 1);
}

int checkUserEnterAnythingYet(char *String)
{
	if (String[0] == '\0')
		return 0;
	return 1;
}

void  OutputTitle(char c[])
{
	system("cls");
	printf("\t\t\t%c%s%c", 175, c, 174);
	printf_s("\n");
}

int Input(char *&name, char c[])
{
	int temp;
	name = new char[10];
	printf("%s: ", c);
	gets_s(name, 10);
	temp = checkUserEnterAnythingYet(name);
	return temp;
}

int ExitAdmin(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	char ch = _getch();
	if (ch == 27)
		adminPointer(f, db, lstAccount, lstCategory, lstSong, fileName);
	if (ch == 49)
		return 0;
}

int isExist(ListCategory lstCategory, char *newCate)
{
	for (int i = 0; i < lstCategory.n; i++)
	{
		if (!_strcmpi(lstCategory.categories[i].name, newCate))
			return 1;
	}
	return 0;
}

int isExist(ListSong lstSong, char *newSong)
{
	for (int i = 0; i < lstSong.n; i++)
	{
		if (!_strcmpi(lstSong.songs[i].name, newSong))
			return 1;
	}
	return 0;
}

int checkStatus(ListCategory lstCategory, ListSong lstSong, char *name, int flag)
{
	switch (flag)
	{
	case 1:
		for (int i = 0; i < lstCategory.n; i++)
		{
			if (!_strcmpi(lstCategory.categories[i].name, name))
			{
				if (lstCategory.categories[i].status == 1)
					return 1;
				return 0;
			}
		}
		break;
	case 2:
		for (int i = 0; i < lstSong.n; i++)
		{
			if (!_strcmpi(lstSong.songs[i].name, name))
			{
				if (lstSong.songs[i].status == 1)
					return 1;
				return 0;
			}
		}
	}
}

void Notify(int flag)
{
	switch (flag)
	{
	case 1:
		printf("Do you want to continue? Press (1)Y or (ESC)N!\n");
		break;
	case 2:
		printf("This name already exists!\n");
		break;
	case 3:
		printf("This name does not exist!\n");
		break;
	case 4:
		printf("Successfully!\n");
		break;
	case 5:
		printf("Not found!\n");
		break;
	case 6:
		printf("Press (1)Y or (ESC)N!\n");
		break;
	case 7:
		printf("This genre has been locked!\n");
		break;
	case 8:
		printf("There are songs in the same genre. Please enter again the category!\n");
		break;
	case 9:
		printf("This song's category has been locked!\n");
		break;
	}	
}

void WriteTotalCate(FILE *f, ListCategory lstCategory, char *newCate)
{
	char *fileNameCate = "data/Bai2_Genres.txt";
	// Code here ĐỌC TẬP TIN, CHẾ ĐỘ GHI TẬP TIN LÀ WT (WRITE TEXT)
	OpenFile(f, fileNameCate, "rt+");
	fprintf_s(f, "%d", lstCategory.n);
	fclose(f);
}

void WriteAddCate(FILE *f, ListCategory lstCategory, char *newCate, char *mode)
{
	char *fileNameCate = "data/Bai2_Genres.txt";
	// Code here ĐỌC TẬP TIN, CHẾ ĐỘ GHI TẬP TIN LÀ WT (WRITE TEXT)
	OpenFile(f, fileNameCate, mode);
	lstCategory.categories[lstCategory.n - 1].id = lstCategory.n;
	lstCategory.categories[lstCategory.n - 1].name = newCate;
	lstCategory.categories[lstCategory.n - 1].status = 1;
	fprintf_s(f, "\n%d,%s,%d,", lstCategory.categories[lstCategory.n - 1].id, lstCategory.categories[lstCategory.n - 1].name, lstCategory.categories[lstCategory.n - 1].status);
	fclose(f);
}

void WriteTotalSong(FILE *f, ListSong lstSong)
{
	char *fileNameCate = "data/Bai2_Songs.txt";
	// Code here ĐỌC TẬP TIN, CHẾ ĐỘ GHI TẬP TIN LÀ WT (WRITE TEXT)
	OpenFile(f, fileNameCate, "rt+");
	fprintf_s(f, "%d", lstSong.n);
	fclose(f);
}

void WriteAddSong(FILE *f, ListSong lstSong, char *mode)
{
	char *fileNameCate = "data/Bai2_Songs.txt";
	// Code here ĐỌC TẬP TIN, CHẾ ĐỘ GHI TẬP TIN LÀ WT (WRITE TEXT)
	OpenFile(f, fileNameCate, mode);
	lstSong.songs[lstSong.n - 1].id = lstSong.n;
	lstSong.songs[lstSong.n - 1].status = 1;
	fprintf_s(f, "\n%d,%d,%s,%d,%d,%d,%d,", lstSong.songs[lstSong.n - 1].id, lstSong.songs[lstSong.n - 1].idCat, lstSong.songs[lstSong.n - 1].name, lstSong.songs[lstSong.n - 1].dateLoad, lstSong.songs[lstSong.n - 1].status);
	fclose(f);
}

void WriteUpdateCate(FILE *f, ListCategory lstCategory, char *cate, char *newCate, char *mode, int flag)
{
	char *fileNameCate = "data/Bai2_Genres.txt";
	// Code here ĐỌC TẬP TIN, CHẾ ĐỘ GHI TẬP TIN LÀ WT (WRITE TEXT)
	OpenFile(f, fileNameCate, mode);
	fprintf_s(f, "%d", lstCategory.n);
	for (int i = 0; i < lstCategory.n; i++)
	{
		if (!_strcmpi(lstCategory.categories[i].name, cate) && flag == 1)
	    	lstCategory.categories[i].name = newCate;
		fprintf_s(f, "\n%d,%s,%d,", lstCategory.categories[i].id, lstCategory.categories[i].name, lstCategory.categories[i].status);
	}
	fclose(f);
}

void WriteUpdateSong(FILE *f, ListSong lstSong, char *mode, char *song, char *newSong)
{
	char *fileName = "data/Bai2_Songs.txt";
	// Code here ĐỌC TẬP TIN, CHẾ ĐỘ GHI TẬP TIN LÀ WT (WRITE TEXT)
	OpenFile(f, fileName, mode);
	fprintf_s(f, "%d", lstSong.n);
	for (int i = 0; i < lstSong.n; i++)
	{
		if (!_strcmpi(lstSong.songs[i].name, song))
			lstSong.songs[i].name = newSong;
		fprintf_s(f, "\n%d,%d,%s,%d,%d,%d,%d,", lstSong.songs[i].id, lstSong.songs[i].idCat, lstSong.songs[i].name, lstSong.songs[i].dateLoad, lstSong.songs[i].status);
	}
	fclose(f);
}

void UpdateCateSong(FILE *f, ListSong lstSong, char *cate, char *newCate, char *mode, int flag)
{
	char *fileNameCate = "data/Bai2_Songs.txt";
	// Code here ĐỌC TẬP TIN, CHẾ ĐỘ GHI TẬP TIN LÀ WT (WRITE TEXT)
	OpenFile(f, fileNameCate, mode);
	fprintf_s(f, "%d", lstSong.n);
	for (int i = 0; i < lstSong.n; i++)
	{
		if (!_strcmpi(lstSong.songs[i].category.name, cate) && flag == 1)
			lstSong.songs[i].category.name = newCate;
		fprintf_s(f, "\n%d,%d,%s,%d,%d,%d,%d,", lstSong.songs[i].id, lstSong.songs[i].idCat, lstSong.songs[i].name, lstSong.songs[i].dateLoad, lstSong.songs[i].status);
	}
	fclose(f);
}

int AddGenres(FILE *f, ListCategory &lstCategory, char *newCate, int flag)
{
	char c;

	if (flag == 0)
	{
		lstCategory.n++;
		WriteTotalCate(f, lstCategory, newCate);
		WriteAddCate(f, lstCategory, newCate, "at");
		OutputPosCate(lstCategory, newCate);
	}

	if (flag == 1)
	{
		Notify(2);
		Notify(1);
		c = _getch();
		if (c == 27)
			flag = 0;
	}	
	return flag;
}

void AddGenresView(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	char *newCate;
	char c;
	int flag = 1;
	do
	{
		flag = 1;
		while (flag)
		{
			newCate = new char[MAXLINE];
			OutputTitle("ADD GENRES");
			printf("Input new genres: ");
			gets_s(newCate, MAXLINE);

			if (!checkUserEnterAnythingYet(newCate))
				continue;
			
			flag = isExist(lstCategory, newCate);
			AddGenres(f, lstCategory, newCate, flag);
			Notify(4);
			Notify(1);
		}	
	} while (ExitAdmin(f, db, lstAccount, lstCategory, lstSong, fileName) == 0);
}

void OutputGenresPos(ListCategory lstCategory, char *cate)
{
	system("cls");
	OutputTitle("LOCK ATITVE GENRES");
	OutputPosCate(lstCategory, cate);
}

int getPos(ListCategory lstCategory, char *findcate)
{
	for (int i = 0; i < lstCategory.n; i++)
	{
		if (!_strcmpi(lstCategory.categories[i].name, findcate))
			return i;
	}
	return -1;
}

int getPos(ListSong lstSong, char *findsong)
{
	for (int i = 0; i < lstSong.n; i++)
	{
		if (!_strcmpi(lstSong.songs[i].name, findsong))
			return i;
	}
	return -1;
}

int checkExist(ListSong lstSong, char *findsong)
{
	for (int i = 0; i < lstSong.n; i++)
	{
		if (!_strcmpi(lstSong.songs[i].name, findsong))
			return 1;
	}
	return -1;
}

int getPosYear(ListSong lstSong, int find)
{
	for (int i = 0; i < lstSong.n; i++)
	{
		if (lstSong.songs[i].dateLoad.year == find)
			return i;
	}
	return -1;
}

int UpdateGenres(FILE *f, ListCategory lstCategory, ListSong lstSong, char *cate)
{
	char c;
	int temp = getPos(lstCategory, cate);
	char *newCate;

	if (temp != -1)
	{
		while (temp)
		{
			newCate = new char[MAXLINE];
			OutputTitle("UPDATE GENRES");
			printf("Input genres edit: %s\n", cate);
			OutputPosCate(lstCategory, cate);
			printf("Input genres update: ");
			gets_s(newCate, MAXLINE);

			if (!checkUserEnterAnythingYet(newCate))
				continue;

			if (isExist(lstCategory, newCate) == 1)
			{
				Notify(2);
				Notify(1);
				c = _getch();
				if (c == 27)
					temp = 0;;
			}
			else
			{			
				WriteUpdateCate(f, lstCategory, cate, newCate, "wt", 1);
				UpdateCateSong(f, lstSong, cate, newCate, "wt", 1);
				OutputGenresPos(lstCategory, newCate);
				temp = 0;
			}
		}
	}

	else
	{
		Notify(3);
		Notify(1);
		c = _getch();
		if (c == 27)
			temp = -1;
	}
	return temp;
}

void UpdateGenresPosView(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	char *findCate;
	int pos = 1;
	do
	{
		pos = 1;
		while (pos)
		{
			findCate = new char[MAXLINE];
	    	OutputTitle("UPDATE GENRES");
			Output(lstCategory);
	     	printf("Input genres edit: ");
	    	gets_s(findCate, MAXLINE);
    
	    	if (!checkUserEnterAnythingYet(findCate))
	    		continue;
			if (!checkStatus(lstCategory, lstSong, findCate, 1))
			{
				Notify(7);
				Notify(1);
				break;
			}

			int temp = UpdateGenres(f, lstCategory, lstSong, findCate);

			if (temp != -1 || temp == 0)
			{
				if (temp == 0)
				{
					Notify(4);
					Notify(1);
				}				
				pos = 0;
			} 				
    	}
	} while (ExitAdmin(f, db, lstAccount, lstCategory, lstSong, fileName) == 0);
}

int getStatus(ListCategory lstCategory, int pos, char *&Char)
{
	Char = new char[10];
	if (lstCategory.categories[pos].status == 1)
	{
		Char = "Lock";
		return 1;
	}		
	else
		Char = "Active";
	return 0;
}

void getStatus(ListSong lstSong, int pos, char *&Char)
{
	Char = new char[10];
	if (lstSong.songs[pos].status == 1)
		Char = "Lock";
	else
		Char = "Active";
}

void CateStatus(FILE *f, ListCategory lstCategory, int status, char *cate, char *mode)
{
	char *fileNameCate = "data/Bai2_Genres.txt";
	// Code here ĐỌC TẬP TIN, CHẾ ĐỘ GHI TẬP TIN LÀ WT (WRITE TEXT)
	OpenFile(f, fileNameCate, mode);
	fprintf_s(f, "%d", lstCategory.n);
	for (int i = 0; i < lstCategory.n; i++)
	{
		if (!_strcmpi(lstCategory.categories[i].name, cate))
		{
			if (lstCategory.categories[i].status == status && status == 1)
		    	if (lstCategory.categories[i].status == 1)
		        	lstCategory.categories[i].status = 0;
			if (lstCategory.categories[i].status == status && status == 0)
				if (lstCategory.categories[i].status == 0)
					lstCategory.categories[i].status = 1;
		}
			
		fprintf_s(f, "\n%d,%s,%d,", lstCategory.categories[i].id, lstCategory.categories[i].name, lstCategory.categories[i].status);
	}
	fclose(f);
}

void SongStatus(FILE *f, ListSong lstSong, int status, char *song, char *mode)
{
	char *fileNameCate = "data/Bai2_Songs.txt";
	// Code here ĐỌC TẬP TIN, CHẾ ĐỘ GHI TẬP TIN LÀ WT (WRITE TEXT)
	OpenFile(f, fileNameCate, mode);
	fprintf_s(f, "%d", lstSong.n);
	for (int i = 0; i < lstSong.n; i++)
	{
		if (!_strcmpi(lstSong.songs[i].category.name, song))
		{
			if (lstSong.songs[i].status == status && status == 1)
				if (lstSong.songs[i].status == 1)
					lstSong.songs[i].status = 0;
			if (lstSong.songs[i].status == status && status == 0)
				if (lstSong.songs[i].status == 0)
					lstSong.songs[i].status = 1;
		}
		fprintf_s(f, "\n%d,%d,%s,%d,%d,%d,%d,", lstSong.songs[i].id, lstSong.songs[i].idCat, lstSong.songs[i].name, lstSong.songs[i].dateLoad, lstSong.songs[i].status);
	}
	fclose(f);
}

void SongStatusV2(FILE *f, ListSong lstSong, char *song, char *mode)
{
	char *fileNameCate = "data/Bai2_Songs.txt";
	// Code here ĐỌC TẬP TIN, CHẾ ĐỘ GHI TẬP TIN LÀ WT (WRITE TEXT)
	OpenFile(f, fileNameCate, mode);
	fprintf_s(f, "%d", lstSong.n);
	for (int i = 0; i < lstSong.n; i++)
	{
		if (!_strcmpi(lstSong.songs[i].name, song))
		{
			if (lstSong.songs[i].status == 1)
				lstSong.songs[i].status = 0;
			else
				lstSong.songs[i].status = 1;
		}
		fprintf_s(f, "\n%d,%d,%s,%d,%d,%d,%d,", lstSong.songs[i].id, lstSong.songs[i].idCat, lstSong.songs[i].name, lstSong.songs[i].dateLoad, lstSong.songs[i].status);
	}
	fclose(f);
}

int LockActiveGenres(FILE *f, ListCategory lstCategory, ListSong lstSong, char *cate, int status)
{
	char c;
	int pos = getPos(lstCategory, cate);
	char *Char;
	status = getStatus(lstCategory, pos, Char);
	if (pos != -1)
	{
		OutputGenresPos(lstCategory, cate);
	    printf("Do you wan to %s for %s? ", Char, cate);
		Notify(6);

		c = _getch();
		if (c == 49)
		{
			CateStatus(f, lstCategory, status, cate, "wt");
			SongStatus(f, lstSong, status, cate, "wt");
			pos = 0;
		}
		else
			pos = -1;
		
	}
	else
	{
		Notify(3);
		Notify(1);
		c = _getch();
		if (c == 27)
			pos = -1;
	}
	return pos;
}

void LockActiveGenresView(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	int pos = 1;
	char *cate;
	do
	{
		pos = 1;
		while (pos)
		{
			cate = new char[MAXLINE];
			OutputTitle("LOCK ATITVE GENRES");
			Output(lstCategory);
			printf("Input genres edit status : ");
			gets_s(cate, MAXLINE);

			int temp = LockActiveGenres(f, lstCategory, lstSong, cate, pos);

			if (temp != -1 || temp == 0)
			{
				if (temp == 0)
				{
					OutputGenresPos(lstCategory, cate);
					Notify(4);
					Notify(1);
				}
				pos = 0;
			}
		}
	} while (ExitAdmin(f, db, lstAccount, lstCategory, lstSong, fileName) == 0);
}

int SelectedGenres(ListCategory lstCategory, ListSong &lstSong, int flags)
{
	char *cate;
	char c;
	int flag = 1;
	while (flag)
	{
		cate = new char[MAXLINE];
		if (flags == 1)
		{
			system("cls");
			OutputTitle("ADD SONG");
			Output(lstCategory);
		}
			
		printf("Selected genres: ");
		gets_s(cate, MAXLINE);

		if (!checkUserEnterAnythingYet(cate))
			continue;

		if (!checkStatus(lstCategory, lstSong, cate, 1))
		{
			Notify(7);
			Notify(1);
			c = _getch();
			if (c == 27)
				return -1;
		}

		else if (getPos(lstCategory, cate) != -1)
		{
			for (int i = 0; i < lstCategory.n; i++)
			{
				if (!_strcmpi(lstCategory.categories[i].name, cate))
				{
					lstSong.songs[lstSong.n - 1].idCat = lstCategory.categories[i].id;
					lstSong.songs[lstSong.n - 1].category.id = lstCategory.categories[i].id;
					lstSong.songs[lstSong.n - 1].category.name = lstCategory.categories[i].name;
					lstSong.songs[lstSong.n - 1].category.status = lstCategory.categories[i].status;
					flag = 0;
					break;
				}
			}
		}
		else
		{
			Notify(3);
			Notify(1);
			c = _getch();
			if (c == 27)
				return -1;
		}
	}
}

int checkSame(ListSong lstSong, char *song)
{
	for (int i = 0; i < lstSong.n; i++)
	{
		if (!_strcmpi(lstSong.songs[i].name, song))
		{
			if (!_strcmpi(lstSong.songs[i].category.name, lstSong.songs[lstSong.n - 1].category.name))
				return 1;
			return 0;
		}		
	}
	return 0;
}

void InputSong(ListCategory lstCategory, ListSong &lstSong, char *&song)
{
	char c;
	int flag = 1;
	while (flag)
	{
		song = new char[MAXLINE];
		printf("Input new song: ");
		gets_s(song, MAXLINE);

		if (!checkUserEnterAnythingYet(song))
			continue;
		while (checkSame(lstSong, song))
		{
			Notify(8);
			SelectedGenres(lstCategory, lstSong, 0);
		}
		lstSong.songs[lstSong.n - 1].name = song;
		flag = 0;
	}
}

void InputDate(ListSong &lstSong)
{
	int day;
	int month;
	int year;

	printf("Input date:\n");
	do
	{
		printf("Day: ");
		scanf_s("%d", &day);
	} while (day < 1 || day > 31);
	
	do
	{
		printf("\nMonth: ");
		scanf_s("%d", &month);
	} while (month < 1 || month > 12);
	
	printf("\nYear: ");
	scanf_s("%d", &year);

	lstSong.songs[lstSong.n - 1].dateLoad.day = day;
	lstSong.songs[lstSong.n - 1].dateLoad.month = month;
	lstSong.songs[lstSong.n - 1].dateLoad.year = year;
}

int AddSong(FILE *f, ListCategory lstCategory, ListSong &lstSong, int &flag)
{
	char c;

	char *song;
	lstSong.n++;
	if (SelectedGenres(lstCategory, lstSong, 1) != -1)
	{
		InputSong(lstCategory, lstSong, song);
		InputDate(lstSong);

		WriteTotalSong(f, lstSong);
		WriteAddSong(f, lstSong, "at");
		OutputPosSong(lstSong, song);
		return 0;
	}
	return 1;
}

void AddSongView(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	char *newSong;
	char c;
	int flag = 1;
	do
	{
		flag = 1;
		while (flag)
		{
			newSong = new char[MAXLINE];
			OutputTitle("ADD SONG");
			flag = AddSong(f, lstCategory, lstSong, flag);
		}
		Notify(4);
		Notify(1);
	} while (ExitAdmin(f, db, lstAccount, lstCategory, lstSong, fileName) == 0);
}

int UpdateSong(FILE *f, ListCategory lstCategory, ListSong lstSong, char *song)
{
	char c;
	int temp = checkExist(lstSong, song);
	char *newSong;

	if (temp != -1)
	{
		while (temp)
		{
			newSong = new char[MAXLINE];
			OutputTitle("UPDATE SONG");
			printf("Input song edit: %s\n", song);
			OutputPosSong(lstSong, song);
			printf("Input song update: ");
			gets_s(newSong, MAXLINE);
			if (!checkUserEnterAnythingYet(newSong))
				continue;
			if (isExist(lstSong, newSong) == 1)
			{
				Notify(2);
				Notify(1);
				c = _getch();
				if (c == 27)
					temp = 0;;
			}
			else
			{
				WriteUpdateSong(f, lstSong, "wt", song, newSong);
				OutputPosSong(lstSong, newSong);
				temp = 0;
			}
		}
	}

	else
	{
		Notify(3);
		Notify(1);
		c = _getch();
		if (c == 27)
			temp = -1;
	}
	return temp;
}

void UpdateNameSong(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	char *findSong;
	int pos = 1;
	while (pos)
	{
		findSong = new char[MAXLINE];
		OutputTitle("UPDATE SONG");
		Output(lstSong);
		printf("Input song edit: ");
		gets_s(findSong, MAXLINE);
		if (!checkUserEnterAnythingYet(findSong))
			continue;
		if (!checkStatus(lstCategory, lstSong, findSong, 2))
		{
			Notify(7);
			Notify(1);
			break;
		}
		int temp = UpdateSong(f, lstCategory, lstSong, findSong);	
		if (temp != -1 || temp == 0)
		{
			if (temp == 0)
			{
				Notify(4);
				Notify(1);
			}
			pos = 0;
		}
	}
}

int UpdateGenre(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *song)
{
	char c;
	int pos = getPos(lstSong, song);
	int temp = 1;
	char *cate;

	if (pos != -1)
	{
		while (temp)
		{
			OutputTitle("UPDATE SONG");
			printf("Input song edit: %s\n", song);
			OutputPosSong(lstSong, song);

			cate = new char[MAXLINE];
			Output(lstCategory);
			printf("Selected genres: ");
			gets_s(cate, MAXLINE);

			if (!checkUserEnterAnythingYet(cate))
				continue;

			if (!checkStatus(lstCategory, lstSong, cate, 1))
			{
				Notify(7);
				Notify(1);
				c = _getch();
				if (c == 27)
					return -1;
				continue;
			}

			if (getPos(lstCategory, cate) != -1)
			{
				for (int i = 0; i < lstCategory.n; i++)
				{
					if (!_strcmpi(lstCategory.categories[i].name, cate))
					{
						lstSong.songs[pos].idCat = lstCategory.categories[i].id;
						lstSong.songs[pos].category.id = lstCategory.categories[i].id;
						lstSong.songs[pos].category.name = lstCategory.categories[i].name;
						lstSong.songs[pos].category.status = lstCategory.categories[i].status;
						UpdateCateSong(f, lstSong, "", "", "wt", 0);
						pos = 0;
						temp = 0;
						break;
					}
				}
				OutputPosSong(lstSong, song);
			}
		}
	}

	else
	{
		Notify(3);
		Notify(1);
		c = _getch();
		if (c == 27)
			pos = -1;
	}
	return pos;
}

void UpdateGenresSong(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong)
{
	char *findSong;
	int pos = 1;
	while (pos)
	{
		findSong = new char[MAXLINE];
		OutputTitle("UPDATE SONG");
		Output(lstSong);
		printf("Input song edit: ");
		gets_s(findSong, MAXLINE);
		if (!checkUserEnterAnythingYet(findSong))
			continue;
		if (!checkStatus(lstCategory, lstSong, findSong, 2))
		{
			Notify(7);
			Notify(1);
			break;
		}
		int temp = UpdateGenre(f, db, lstAccount, lstCategory, lstSong, findSong);
		if (temp != -1 || temp == 0)
		{
			if (temp == 0)
			{
				Notify(4);
				Notify(1);
			}
			pos = 0;
		}
	}
}

int LockActiveSong(FILE *f, ListCategory lstCategory, ListSong lstSong, char *song, int status)
{
	char c;
	int pos = getPos(lstSong, song);
	char *Char;
	char *cate;
	getStatus(lstSong, pos, Char);
	if (pos != -1)
	{
		OutputPosSong(lstSong, song);
		cate = lstSong.songs[pos].category.name;
		if (!checkStatus(lstCategory, lstSong, cate, 1))
		{
			Notify(9);
			Notify(1);
		}
		else
		{
			printf("Do you wan to %s for %s? ", Char, song);
			Notify(6);

			c = _getch();
			if (c == 49)
				SongStatusV2(f, lstSong, song, "wt");
			pos = 0;
		}
	}
	else
	{
		Notify(3);
		Notify(1);
		c = _getch();
		if (c == 27)
			pos = -1;
	}
	return pos;
}

void UpdateSongPosView(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	int flag;
	do
	{
		do
		{
			OutputTitle("UPDATE SONG");
			printf("Select Options:\n");
			printf("1. Update name song!\n");
			printf("2. Update genres song!\n");
			scanf_s("%d", &flag);
			getchar();
		} while (flag < 1 || flag > 2);
		if (flag == 1)
			UpdateNameSong(f, db, lstAccount, lstCategory, lstSong, fileName);
		if (flag == 2)
			UpdateGenresSong(f, db, lstAccount, lstCategory, lstSong);
	} while (ExitAdmin(f, db, lstAccount, lstCategory, lstSong, fileName) == 0);
}

void LockActiveSongView(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	int pos = 1;
	char *song;
	do
	{
		pos = 1;
		while (pos)
		{
			song = new char[MAXLINE];
			OutputTitle("LOCK ATITVE SONG");
			Output(lstSong);
			printf("Input song edit status : ");
			gets_s(song, MAXLINE);

			int temp = LockActiveSong(f, lstCategory, lstSong, song, pos);

			if (temp != -1 || temp == 0)
			{
				if (temp == 0)
				{
					OutputPosSong(lstSong, song);
					Notify(4);
					Notify(1);
				}
				pos = 0;
			}
		}
	} while (ExitAdmin(f, db, lstAccount, lstCategory, lstSong, fileName) == 0);
}

void OutputActivedLock(ListSong lstSong, int flag)
{
	OutputTitle(5, 1);
	for (int i = 0; i < lstSong.n; i++)
	{
		if (flag == 1)
		{
			if (lstSong.songs[i].status == 1)
			{
				DrawSTT();
				DrawRow(84, 21, 204, 206, 205, 185, 0);
				DrawSTT(lstSong.songs[i].id);
				DrawNameGenres(lstSong.songs[i].category.name);
				DrawNameSongs(lstSong.songs[i].name);
				DrawNameDate(lstSong, i);
				DrawStatus(lstSong.songs[i].status, 1);
			}
		}
		else
		{
			if (lstSong.songs[i].status == 0)
			{
				DrawSTT();
				DrawRow(84, 21, 204, 206, 205, 185, 0);
				DrawSTT(lstSong.songs[i].id);
				DrawNameGenres(lstSong.songs[i].category.name);
				DrawNameSongs(lstSong.songs[i].name);
				DrawNameDate(lstSong, i);
				DrawStatus(lstSong.songs[i].status, 1);
			}
		} 	
	}
	DrawRow(105, 21, 200, 202, 205, 188, 1);
}

void FilteringListSongs(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	int flag;
	char c = 49;

	do
	{
		do
		{
			OutputTitle("LIST");
			printf("Selected View:\n");
			printf("1. List songs actived!\n");
			printf("2. List songs locked!\n");
			printf("Options:");
			scanf_s("%d", &flag);
		} while (flag < 1 || flag > 2);

		if (flag == 1)
		{
			OutputTitle("LIST");
			OutputActivedLock(lstSong, 1);
		}
		if (flag == 2)
		{
			OutputTitle("LIST");
			OutputActivedLock(lstSong, 0);
		}
		Notify(1);
	} while (ExitAdmin(f, db, lstAccount, lstCategory, lstSong, fileName) == 0);
}


void ShowView(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	int flag;
	char c = 49;

	do
	{
		do
		{
			OutputTitle("LIST");
			printf("Selected View:\n");
			printf("1. Genres!\n");
			printf("2. Songs!\n");
			printf("Options:");
			scanf_s("%d", &flag);
		} while (flag < 1 || flag > 2);

		if (flag == 1)
		{
			OutputTitle("LIST");
			Output(lstCategory);
		}
		if (flag == 2)
		{
			OutputTitle("LIST");
			Output(lstSong);
		}
		Notify(1);
	} while (ExitAdmin(f, db, lstAccount, lstCategory, lstSong, fileName) == 0);
}


int isLogin()
{
	if (Compare(username, "admin") == 0 && Compare(password, "admin") == 0)
		return 1;
	if (Compare(username, "user") == 0 && Compare(password, "user") == 0)
		return 2;
	return 0;
}

void Load(ListAccount &lstAccount, Database db)
{
	char *temp;
	char delim = ',';
	char *next;
	int idx = 0;
	lstAccount.n = db.iLines;
	for (int i = 0; i < db.iLines; i++)
	{	
		while (lstAccount.n != NULL)
		{
			temp = TachTheoKiTu(db.database[i], delim, next);
			lstAccount.account[idx].username = temp;
			temp = TachTheoKiTu(NULL, delim, next);
			lstAccount.account[idx].password = temp;
			temp = TachTheoKiTu(NULL, delim, next);
			lstAccount.account[idx].status = ChuyenDoiChuoiSo(temp);
			idx++;
			if (*next == '\n' || *next == '\0')
				break;
		}
	}
}

void Input(FILE *f, ListAccount &lstAccount, Database db, char *fileName)
{
	Input(f, db, fileName, "rt");
	Load(lstAccount, db);
}

void Login(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	switch (isLogin())
	{
	case 0:
		loginPointer(f, db, lstAccount, lstCategory, lstSong, fileName);
		break;
	case 1:
		adminPointer(f, db, lstAccount, lstCategory, lstSong, fileName);
		break;
	case 2:
		userPointer(f, db, lstAccount, lstCategory, lstSong, fileName);
	}
}

void loginView(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{	
	int count = 0;
	do
	{
		OutputTitle("LOGIN");
		if (count != 0)
			puts("Incorrect account!");
		count++;
		if (Input(username, "Username") == 0)
			continue;
		InputPass(password, "Password");
	} while (isLogin() == 0);
	Login(f, db, lstAccount, lstCategory, lstSong, fileName);
}

void AdminMenu(int &opt)
{
	do
	{
		OutputTitle("MENU");
		printf("1. Add a new song genre!\n");
		printf("2. Update song genre at position!\n");
		printf("3. Lock or Active song genre!\n");
		printf("4. Add a new song!\n");
		printf("5. Update the song at the position!\n");
		printf("6. Lock or Active a song!\n");
		printf("7. Filtering a playlist!\n");
		printf("8. Show List!\n");
		printf("Options: ");
		opt = _getch();
		if (opt == 27)
			break;
	} while (opt < 49 || opt > 56);
	system("cls");
}

void AdminView(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	system("cls");
	int opt = 0;
	AdminMenu(opt);

	switch (opt)
	{
	case 49:
		AddGenresView(f, db, lstAccount, lstCategory, lstSong, fileName);
		break;
	case 50:
		UpdateGenresPosView(f, db, lstAccount, lstCategory, lstSong, fileName);
		break;
	case 51:
		LockActiveGenresView(f, db, lstAccount, lstCategory, lstSong, fileName);
		break;
	case 52:
		AddSongView(f, db, lstAccount, lstCategory, lstSong, fileName);
		break;
	case 53:
	    UpdateSongPosView(f, db, lstAccount, lstCategory, lstSong, fileName);
		break;
	case 54:
		LockActiveSongView(f, db, lstAccount, lstCategory, lstSong, fileName);
		break;
	case 55:
		FilteringListSongs(f, db, lstAccount, lstCategory, lstSong, fileName);
		break;
	case 56:
		ShowView(f, db, lstAccount, lstCategory, lstSong, fileName);
		break;
	case 27:
		loginPointer(f, db, lstAccount, lstCategory, lstSong, fileName);
		break;
	}
}

int ExitUser(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	char ch = _getch();
	if (ch == 27)
		userPointer(f, db, lstAccount, lstCategory, lstSong, fileName);
	if (ch == 49)
		return 0;
}

int searchGenres(FILE *f, ListCategory lstCategory, ListSong lstSong, char *cate)
{
	char c;
	int temp = getPos(lstCategory, cate);

	if (temp != -1)
	{
		OutputTitle(5, 1);
		for (int i = 0; i < lstSong.n; i++)
		{
			if (!_strcmpi(lstSong.songs[i].category.name, cate))
		    	if (lstSong.songs[i].status == 1)
		    	{
			    	DrawSTT();
				    DrawRow(84, 21, 204, 206, 205, 185, 0);
			    	DrawSTT(lstSong.songs[i].id);
			    	DrawNameGenres(lstSong.songs[i].category.name);
			    	DrawNameSongs(lstSong.songs[i].name);
			    	DrawNameDate(lstSong, i);
			    	DrawStatus(lstSong.songs[i].status, 1);
		    	}
		}
		DrawRow(105, 21, 200, 202, 205, 188, 1);
		temp = 0;
	}
	else
	{
		Notify(3);
		Notify(1);
		c = _getch();
		if (c == 27)
			temp = -1;
	}
	return temp;
}

void SearchSongsGenresView(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	char *findCate;
	int pos = 1;
	do
	{
		pos = 1;
		while (pos)
		{
			findCate = new char[MAXLINE];
			OutputTitle("SEARCH SONG BY GENRES");
			Output(lstCategory);
			printf("Input genres: ");
			gets_s(findCate, MAXLINE);

			if (!checkUserEnterAnythingYet(findCate))
				continue;
			if (!checkStatus(lstCategory, lstSong, findCate, 1))
			{
				Notify(7);
				Notify(1);
				break;
			}

			int temp = searchGenres(f, lstCategory, lstSong, findCate);

			if (temp != -1 || temp == 0)
			{
				if (temp == 0)
					Notify(1);
				pos = 0;
			}
		}
	} while (ExitUser(f, db, lstAccount, lstCategory, lstSong, fileName) == 0);
}

int searchYears(FILE *f, ListCategory lstCategory, ListSong lstSong, int find)
{
	char c;
	int temp = getPosYear(lstSong, find);

	if (temp != -1)
	{
		OutputTitle(5, 1);
		for (int i = 0; i < lstSong.n; i++)
		{
			if (lstSong.songs[i].dateLoad.year == find)
				if (lstSong.songs[i].status == 1)
				{
					DrawSTT();
					DrawRow(84, 21, 204, 206, 205, 185, 0);
					DrawSTT(lstSong.songs[i].id);
					DrawNameGenres(lstSong.songs[i].category.name);
					DrawNameSongs(lstSong.songs[i].name);
					DrawNameDate(lstSong, i);
					DrawStatus(lstSong.songs[i].status, 1);
				}
		}
		DrawRow(105, 21, 200, 202, 205, 188, 1);
		temp = 0;
	}
	else
	{
		Notify(3);
		Notify(1);
		c = _getch();
		if (c == 27)
			temp = -1;
	}
	return temp;
}

void SearchSongsYearsView(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	int find;
	int pos = 1;
	do
	{
		pos = 1;
		while (pos)
		{
			OutputTitle("search SONG BY YEARS");
			printf("Input year: ");
			scanf_s("%d", &find);

			int temp = searchYears(f, lstCategory, lstSong, find);

			if (temp != -1 || temp == 0)
			{
				if (temp == 0)
					Notify(1);
				pos = 0;
			}
		}
	} while (ExitUser(f, db, lstAccount, lstCategory, lstSong, fileName) == 0);
}

void UserMenu(int &opt)
{
	do
	{
		OutputTitle("MENU");
		printf("1. List songs by genre!\n");
		printf("2. List songs by year!\n");
		printf("Options: ");
		opt = _getch();
		if (opt == 27)
			break;
	} while (opt < 49 || opt > 50);
	system("cls");
}

void UserView(FILE *f, Database db, ListAccount &lstAccount, ListCategory lstCategory, ListSong lstSong, char *fileName)
{
	system("cls");
	int opt = 0;
	UserMenu(opt);

	switch (opt)
	{
	case 49:
		SearchSongsGenresView(f, db, lstAccount, lstCategory, lstSong, fileName);
		break;
	case 50:
		SearchSongsYearsView(f, db, lstAccount, lstCategory, lstSong, fileName);
		break;
	case 27:
		loginPointer(f, db, lstAccount, lstCategory, lstSong, fileName);
		break;
	}
}

int main()
{
	SetConsoleCP(437);
	SetConsoleOutputCP(437);

	loginPointer = loginView;
	adminPointer = AdminView;
	userPointer = UserView;

	ListCategory lstCategory;
	ListSong lstSong;
	Database db;
	FILE *f = NULL;
	ListAccount lstAccount;

	char *fileName = "data/Bai2_Accounts.txt";
	char *fileNameCate = "data/Bai2_Genres.txt";
	char *fileNameSong = "data/Bai2_Songs.txt";

	char *mode = "rt";

	Input(f, db, fileNameCate, mode);
	Load(lstCategory, db);

	Input(f, db, fileNameSong, mode);
	Load(lstCategory, lstSong, db);

	loginPointer(f, db, lstAccount, lstCategory, lstSong, fileName);
    _getch();
}