#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

char* Buffer;
long long razmer = 0;

struct LIST
{
	unsigned char* before;
	unsigned char* after;
};

struct CyclesVariables
{
	char ConStart[30];//начало
	char ConMid[30];//тело цикла
	char ConEnd[30];//конец
	char VarAction[30];//операция
	char VarValue[30];//значение переменной
	char VarName[30];//имя сгенерированной переменной
	char FunName[30];//имя функции
};

void DeleteSpaces(FILE* First, FILE* Second)
{
	int i = 0;
	int c, prev;
	while ((c = fgetc(First)) != EOF)
	{
		if (c != '\n' && c != '\t' && c != ' ') fputc(c, Second);
		else prev = c;

		if (c == '\t' || c == ' ' || c == '\n')
		{
			int i = 0;
			while (!isgraph(c) && !feof(First))//не пишем ненужные символы
			{
				c = fgetc(First);
				i++;
			}
			if (i == 1 || (prev == '\n' && i >= 2)) fputc(prev, Second);//переход на новую строку
			if (!feof(First)) fseek(First, -1, SEEK_CUR);
		}

		else if (c == '\'')
			do
			{
				prev = c;
				c = fgetc(First);
				fputc(c, Second);
				if (prev == '\\')
				{
					prev = c;
					c = fgetc(First);
					fputc(c, Second);
				}
			} while (c != '\'');

		else if (c == '"')
			do
			{
				prev = c;
				c = fgetc(First);
				fputc(c, Second);
				if (prev == '\\')
				{
					prev = c;
					c = fgetc(First);
					fputc(c, Second);
				}
			} while (c != '"');


		else if (c == '(')
		{
			int j = 1;//проверка вхождения
			while (j != 0)
			{
				c = fgetc(First);
				if (c == EOF) goto DONE;
				fputc(c, Second);
				if (c == '\'')
					do
					{
						prev = c;
						c = fgetc(First);
						fputc(c, Second);
						if (prev == '\\')
						{
							prev = c;
							c = fgetc(First);
							fputc(c, Second);
						}
					} while (c != '\'');
				else if (c == '"')
					do
					{
						prev = c;
						c = fgetc(First);
						fputc(c, Second);
						if (prev == '\\')
						{
							prev = c;
							c = fgetc(First);
							fputc(c, Second);
						}
					} while (c != '"');
				else if (c == '(') j++;
				else if (c == ')') j--;
			}
		}

		else if (c == '#')
		{
			while ((c = fgetc(First)) != '\n') fputc(c, Second);
			fputc(c, Second);
		}
	}
DONE:
	fclose(First);
	fclose(Second);
}

void Function_Or_Var_Rewrite(FILE* First, FILE* Second, int* fun, int* i, char* Check)
{
	int k = -1;
	char g;
	int l = 0;
	char* TakeCare;
	TakeCare = (char*)malloc(sizeof(char) * 2);
	TakeCare[0] = '\0';
	while ((g = fgetc(First)) != '=' && g != '(' && g != ')' && g != ';')
	{
		TakeCare[l] = g;
		l++;
		TakeCare = (char*)realloc(TakeCare, sizeof(char) * (l + 2));
		if (g == '\n') k--;
		k--;
	}
	TakeCare[l] = '\0';

	if (g == '=' || g == ';')
	{
		fseek(First, k, SEEK_CUR);
		//fprintf(Second, "%s", Check);
		fputs(Check, Second);
		while ((g = fgetc(First)) != ';') fputc(g, Second);
		fputc(g, Second);
	}

	else if (g == '(')
	{
		FILE* Buffer;
		char FunNum[8] = "Fun";
		char Number[3] = { '\0' };
		_itoa(*fun, Number, 10);
		(*fun)++;
		strncat(FunNum, Number, strlen(Number));
		strncat(FunNum, ".c", strlen(".c"));
		fopen_s(&Buffer, FunNum, "w");
		fprintf(Buffer, "%s%s%c", Check, TakeCare, g);
		while ((g = fgetc(First)) != '{') fputc(g, Buffer);
		fputc(g, Buffer);
		(*i)++;
		while (*i > 0 && !feof(First))//пока внутри функции
		{
			g = fgetc(First);
			if (g != EOF) fputc(g, Buffer);
			if (g == '\'')
			{
				while ((g = fgetc(First)) != '\'')
				{
					if (g == '\\')
					{
						fputc(g, Buffer);
						g = fgetc(First);
					}
					fputc(g, Buffer);
				}
				fputc(g, Buffer);
			}
			else if (g == '\"')
			{
				while ((g = fgetc(First)) != '\"')
				{
					if (g == '\\')
					{
						fputc(g, Buffer);
						g = fgetc(First);
					}
					fputc(g, Buffer);
				}
				fputc(g, Buffer);
			}

			else if (g == '{') (*i)++;
			else if (g == '}') (*i)--;
		}
		fclose(Buffer);
	}
	free(TakeCare);
	TakeCare = NULL;
}

int GenerateFunctionFiles(FILE* First, FILE* Second)
{
	int c;
	int i = 0;//вхождение в функцию
	int fun = 0;//кол-во функций

	while ((c = fgetc(First)) != EOF && !feof(First))
	{
		if (c == '#')
		{
			do
			{
				fputc(c, Second);
			} while ((c = fgetc(First)) != '\n');
			fputc(c, Second);
		}

		else if (c == '{') i++;
		else if (c == '}') i--;

		else if (i == 0 && c == 'v')//потенциально void
		{
			char Check[6] = { '\0' };
			Check[0] = c;
			for (int j = 1; strstr("void ", Check) && j < 5; j++) Check[j] = fgetc(First);
			if (strcmp("void ", Check) == 0) Function_Or_Var_Rewrite(First, Second, &fun, &i, &Check[0]);
		}

		else if (i == 0 && c == 'i')//потенциально int
		{
			char Check[5] = { '\0' };
			Check[0] = c;
			for (int j = 1; strstr("int ", Check) && j < 4; j++) Check[j] = fgetc(First);
			if (strcmp("int ", Check) == 0 || strcmp("int*", Check) == 0) Function_Or_Var_Rewrite(First, Second, &fun, &i, Check);
		}

		else if (i == 0 && c == 'c')//потенциально char
		{
			char Check[6] = { '\0' };
			Check[0] = c;
			for (int j = 1; strstr("char ", Check) && j < 5; j++) Check[j] = fgetc(First);
			if (strcmp("char ", Check) == 0 || strcmp("char*", Check) == 0) Function_Or_Var_Rewrite(First, Second, &fun, &i, Check);
		}

		else if (i == 0 && c == 's')//потенциально структура
		{
			char Check[8] = { '\0' };
			Check[0] = c;
			for (int j = 1; strstr("struct ", Check) && j < 7; j++) Check[j] = fgetc(First);
			if (strcmp("struct ", Check) == 0)
			{
				int k = -1;
				char g;
				while ((g = fgetc(First)) != '{' && g != ')')
				{
					if (g == '\n') k--;
					k--;
				}
				if (g == '{')
				{
					fseek(First, k, SEEK_CUR);
					fprintf(Second, "\n%s", Check);
					while ((g = fgetc(First)) != ';' || i > 0)
					{
						fputc(g, Second);
						if (g == '{') i++;
						if (g == '}') i--;
					}
					fputc(g, Second);
				}
			}
		}

		else if (i == 0 && c == 'f')//потенциально float
		{
			char Check[7] = { '\0' };
			Check[0] = c;
			for (int j = 1; strstr("float ", Check) && j < 6; j++) Check[j] = fgetc(First);
			if (strcmp("float ", Check) == 0 || strcmp("float*", Check) == 0) Function_Or_Var_Rewrite(First, Second, &fun, &i, Check);

		}

		else if (i == 0 && c == 'd')//потенциально double
		{
			char Check[8] = { '\0' };
			Check[0] = c;
			for (int j = 1; strstr("double ", Check) && j < 7; j++) Check[j] = fgetc(First);
			if (strcmp("double ", Check) == 0 || strcmp("double*", Check) == 0) Function_Or_Var_Rewrite(First, Second, &fun, &i, Check);
		}

		else if (i == 0 && c == 'l')
		{
			char Check[6] = { '\0' };
			Check[0] = c;
			for (int j = 1; strstr("long ", Check) && j < 5; j++) Check[j] = fgetc(First);
			if (strcmp("long ", Check) == 0 || strcmp("long*", Check) == 0) Function_Or_Var_Rewrite(First, Second, &fun, &i, Check);
		}

	}
	fclose(First);
	fclose(Second);
	return fun;
}

void WriteCycles(FILE* First, FILE* Second, struct CyclesVariables* Generate, int Limit)
{
	int count = 0;
	char Temp[30] = { '\0' };
	//ищем первое объявление функции в программе, чтобы перед ней объявить глобальные переменные типа Int
	do
	{
		fputs(Temp, Second);
		char c;
		c = fgetc(First);
		fputc(c, Second);
		fscanf(First, "%s", Temp);
	} while (!strstr(Temp, "int") && !strstr(Temp, "void") && !strstr(Temp, "char") && !strstr(Temp, "float") && !strstr(Temp, "struct"));


	for (int i = 0; i < Limit; i += 2)//мусорные переменные
	{
		fprintf(Second, "int %s = %s;\n", Generate[i].VarName, Generate[i].VarValue);
		if (i + 1 < Limit)fprintf(Second, "int %s = %s;\n", Generate[i + 1].VarName, Generate[i + 1].VarValue);
		else break;
	}
	fputs(Temp, Second);

	//в Temp уже записана функция/структура, и здесь выполняется проверка на вход в структуру (чтобы не записать мусор в нее)
	if (strcmp("struct", Temp) == 0)
	{
		int c;
		int j = 0;
		int flag = 0;
		while (j != 0 || flag == 0)//пока внутри {}
		{
			c = fgetc(First);
			fputc(c, Second);
			if (c == '{')
			{
				j++;
				flag = 1;
			}
			else if (c == '}') j--;
		}
	}

	int i = 0;//проверка на вхождение в {}
	int flag = 0;
	int reason = 1;//вход в main
	int CallFunctions = 0;
	int c, prev;

	while ((c = fgetc(First)) != EOF)
	{
	Breakout:
		fputc(c, Second);

		if (c == ';' && i != 0) // находимся внутри функции после знака ";"
		{
			while (isspace(c = fgetc(First)) && !feof(First)) fputc(c, Second);
			char Check[5] = { '\0' };
			Check[0] = c;

			for (int j = 1; strstr("else", Check) != NULL && j < 4 && !feof(First); j++)
			{
				c = fgetc(First);
				Check[j] = c;
			}

			if (strcmp("else", Check) != 0)//если после ; не else, то пишем мусор
			{
				int VariableSet = rand() % (Limit - 1);
				fprintf(Second, "%s%s%s%s", Generate[VariableSet].ConStart, Generate[VariableSet].ConMid, Generate[VariableSet].ConEnd, Generate[VariableSet].VarAction);
				if (CallFunctions && count >= 0)
				{
					fprintf(Second, "%s();", Generate[count].FunName);
					count--;
				}
			}
			if (strchr(Check, '{') != NULL) i++;
			if (strchr(Check, '}') != NULL)
			{
				i--;
				if (i == 0 && count < Limit) flag = 1;
			}
			if (strchr(Check, '/') != NULL)
				for (int k = 0; k < 5; k++)
					if (Check[k] != '/') fputc(Check[k], Second);
					else
					{
						c = '/';
						goto Breakout;
					}
			fputs(Check, Second);

			if (flag && reason)
			{
				flag = 0;
				fprintf(Second, "void %s(){%s%s%s%s}", Generate[count].FunName, Generate[count].ConStart, Generate[count].ConMid, Generate[count].ConEnd, Generate[count].VarAction);
				count++;
			}
		}

		else if (c == '(')
		{
			int j = 1;
			while (j != 0)//пока не вышли из скобки
			{
				c = fgetc(First);
				fputc(c, Second);
				if (c == '\'')
					do
					{
						prev = c;
						c = fgetc(First);
						fputc(c, Second);
						if (prev == '\\')
						{
							prev = c;
							c = fgetc(First);
							fputc(c, Second);
						}
					} while (c != '\'');
				else if (c == '"')
					do
					{
						prev = c;
						c = fgetc(First);
						fputc(c, Second);
						if (prev == '\\')
						{
							prev = c;
							c = fgetc(First);
							fputc(c, Second);
						}
					} while (c != '"');
				else if (c == '(') j++;
				else if (c == ')') j--;
			}
		}

		else if (c == '{') i++;

		else if (c == '}')
		{
			i--;
			if (i == 0 && count < Limit && reason)//мусорная функция
			{
				fprintf(Second, "void %s(){%s%s%s%s}", Generate[count].FunName, Generate[count].ConStart, Generate[count].ConMid, Generate[count].ConEnd, Generate[count].VarAction);
				count++;
			}
		}

		else if (c == '\'')
			do
			{
				prev = c;
				c = fgetc(First);
				fputc(c, Second);
				if (prev == '\\')
				{
					prev = c;
					c = fgetc(First);
					fputc(c, Second);
				}
			} while (c != '\'');

		else if (c == '"')
			do
			{
				prev = c;
				c = fgetc(First);
				fputc(c, Second);
				if (prev == '\\')
				{
					prev = c;
					c = fgetc(First);
					fputc(c, Second);
				}
			} while (c != '"');

		else if (c == 's' && i == 0) //потенциально структура
		{
			char Check[7] = { '\0' };
			Check[0] = c;
			for (int j = 1; strstr("struct", Check) != NULL && j < 6 && !feof(First); j++)
			{
				Check[j] = fgetc(First);
				fputc(Check[j], Second);
			}

			if (strcmp("struct", Check) == 0)
			{
				int j = 0;
				int f = 0;
				while (j != 0 || f == 0)
				{
					c = fgetc(First);
					fputc(c, Second);
					if (c == '{')
					{
						j++;
						f = 1;
					}
					else if (c == '}') j--;
				}
			}
		}

		else if (c == 'm' && i == 0) //потенциально main
		{
			char Check[5] = { '\0' };
			Check[0] = c;
			for (int j = 1; strstr("main", Check) != NULL && j < 4 && !feof(First); j++)
			{
				Check[j] = fgetc(First);
				fputc(Check[j], Second);
			}
			if (strcmp("main", Check) == 0)
			{
				count--;
				CallFunctions = 1;
				reason = 0;
			}
		}
	}

	fclose(First);
	fclose(Second);
}

void GenerateCycles(int j, struct CyclesVariables* Generate)
{
	int Length;
	int l;
	for (int i = 0; i < j; i++)
	{
		int Rand = rand() % 20 + 25;

		memset(Generate[i].ConStart, '\0', 30 * sizeof(char));
		memset(Generate[i].ConMid, '\0', 30 * sizeof(char));
		memset(Generate[i].ConEnd, '\0', 30 * sizeof(char));
		memset(Generate[i].VarAction, '\0', 30 * sizeof(char));
		memset(Generate[i].VarValue, '\0', 30 * sizeof(char));
		memset(Generate[i].VarName, '\0', 30 * sizeof(char));
		memset(Generate[i].FunName, '\0', 30 * sizeof(char));

		//Генерируем цикл 
		if (Rand % 3 == 0)
		{
			strcat(Generate[i].ConStart, "if(");
			strcat(Generate[i].ConEnd, ")");
		}
		else if (Rand % 3 == 1)
		{
			strcat(Generate[i].ConStart, "while(");
			strcat(Generate[i].ConEnd, ")");
		}
		else if (Rand % 3 == 2)
		{
			strcat(Generate[i].ConStart, "for(;");
			strcat(Generate[i].ConEnd, ";)");
		}

		Length = rand() % 10 + 5;//длина имени
		l = 0;
		//Геенрируем имя переменной
		for (int k = 0; k < Length; k++, l++)
		{
			char c;
			int p = rand() % 532 + 67;
			if (p % 3 == 0) c = rand() % 25 + 65; // заглавная буква
			else if (p % 3 == 1) c = rand() % 25 + 97; //строчная буква
			else if (l != 0) c = rand() % 9 + 48; //c - число по таблице ASCII
			else c = '_';
			Generate[i].VarName[l] = c;
		}

		Length = rand() % 10 + 5;
		l = 0;

		for (int k = 0; k < Length; k++, l++)
		{
			char c;
			int p = rand() % 532 + 67;
			if (p % 3 == 0) c = rand() % 25 + 65;
			else if (p % 3 == 1) c = rand() % 25 + 97;
			else if (l != 0) c = rand() % 9 + 48;
			else c = '_';
			Generate[i].FunName[l] = c;
		}

		Rand = rand() % 9000 + 1000;
		_itoa(Rand, Generate[i].VarValue, 10);//переводим значение переменной в строку
		Length = rand() % 9000 + 1000;
		strcat(Generate[i].ConMid, Generate[i].VarName);
		char Temp[6] = { '\0' };
		_itoa(Length, Temp, 10);

		if (Length >= Rand)
		{
			strcat(Generate[i].ConMid, "<=");
			strcat(Generate[i].ConMid, Temp);
			strcat(Generate[i].VarAction, Generate[i].VarName);
			Rand = rand() % 2;
			if (Rand == 0) strcat(Generate[i].VarAction, "++;");
			else strcat(Generate[i].VarAction, "*=10;");
		}
		else
		{
			strcat(Generate[i].ConMid, ">");
			strcat(Generate[i].ConMid, Temp);
			strcat(Generate[i].VarAction, Generate[i].VarName);
			Rand = rand() % 2;
			if (Rand == 0) strcat(Generate[i].VarAction, "--;");
			else strcat(Generate[i].VarAction, "/=10;");
		}
	}
}

void CountLetters(FILE* File, int* a)//считаем кол-во символов в строке
{
	int i = 0;
	rewind(File);
	unsigned char c = fgetc(File);
	while (!feof(File))
	{
		while (c != '\n' && !feof(File))
		{
			c = fgetc(File);
			a[i]++;
		}
		c = fgetc(File);
		a[i]++;
		i++;
	}
}

void RenameVariables(int lines)
{
	FILE** Alphabet;
	FILE** Modify;
	int* A;//массивы кол-ва символов
	int* B;


	Alphabet = (FILE**)malloc(sizeof(FILE*) * 2);
	fopen_s(&Alphabet[0], "Before.txt", "r");
	fopen_s(&Alphabet[1], "After.txt", "r");


	A = (int*)calloc(lines, sizeof(int));
	B = (int*)calloc(lines, sizeof(int));
	CountLetters(Alphabet[0], A);
	CountLetters(Alphabet[1], B);


	struct LIST* Words;
	Words = (struct LIST*)malloc(sizeof(struct LIST) * lines);
	rewind(Alphabet[0]);
	rewind(Alphabet[1]);

	for (int i = 0; i < lines; i++)//считываем названия
	{
		Words[i].before = (unsigned char*)malloc(A[i] * sizeof(unsigned char));
		Words[i].after = (unsigned char*)malloc(B[i] * sizeof(unsigned char));
		fread(Words[i].before, sizeof(unsigned char), A[i], Alphabet[0]);
		fread(Words[i].after, sizeof(unsigned char), B[i], Alphabet[1]);
		Words[i].before[A[i] - 1] = '\0';
		Words[i].after[B[i] - 1] = '\0';
	}

	fclose(Alphabet[0]);
	fclose(Alphabet[1]);
	free(Alphabet);
	Alphabet = NULL;
	remove("Before.txt");
	remove("After.txt");

	free(A);
	free(B);
	A = NULL;
	B = NULL;

	Modify = (FILE**)malloc(sizeof(FILE*) * 2);
	fopen_s(&Modify[0], "Final.c", "r");
	fopen_s(&Modify[1], "Draft.c", "w");

	unsigned char* buf;
	buf = (unsigned  char*)malloc(2 * sizeof(unsigned  char));
	buf[0] = '\0';

	while (!feof(Modify[0]))
	{
		unsigned char c;
		int n = 0;

		while (!isalpha(c = fgetc(Modify[0])) && c != '_' && !feof(Modify[0]))//пропускаем код
		{
			char prev;
			fputc(c, Modify[1]);

			if (c == '"')
				do
				{
					prev = c;
					c = fgetc(Modify[0]);
					fputc(c, Modify[1]);
					if (prev == '\\')
					{
						prev = c;
						c = fgetc(Modify[0]);
						fputc(c, Modify[1]);
					}
				} while (c != '"');
			else if (c == '\'')
				do
				{
					prev = c;
					c = fgetc(Modify[0]);
					fputc(c, Modify[1]);
					if (prev == '\\')
					{
						prev = c;
						c = fgetc(Modify[0]);
						fputc(c, Modify[1]);
					}
				} while (c != '\'');
		}

		while ((c == '_' || isalpha(c) || isdigit(c)) && !feof(Modify[0]))//переменная
		{
			buf[n] = c;
			n++;
			buf = (unsigned char*)realloc(buf, n + 2);
			c = fgetc(Modify[0]);
		}
		buf[n] = '\0';

		for (int k = 0; k < lines; k++)
			if (strcmp(buf, Words[k].before) == 0 && strcmp(buf, "main") != 0 && strcmp(buf, "char") != 0 && strcmp(buf, "int") && strcmp(buf, "float") != 0 != 0 && strcmp(buf, "void") != 0 && strcmp(buf, "struct") != 0 && strcmp(buf, "FILE") != 0)
			{
				fputs(Words[k].after, Modify[1]);
				memset(buf, NULL, strlen(buf) + 1);
				break;
			}

		if (buf != NULL)
		{
			fputs(buf, Modify[1]);
			memset(buf, NULL, strlen(buf) + 1);
		}
		if (!feof(Modify[0])) fputc(c, Modify[1]);
	}

	fclose(Modify[0]);
	fclose(Modify[1]);
	free(Modify);
	Modify = NULL;
}

int CreateVariables(FILE* Read)
{
	FILE* Before;
	FILE* After;
	int LinesCount = 0, i;
	int s1;
	fopen_s(&Before, "Before.txt", "w");
	fopen_s(&After, "After.txt", "w");
	short FlagFunction = 0;

	int c;
	for (razmer; (c = fgetc(Read)) != EOF; razmer++)
	{
		Buffer = (char*)realloc(Buffer, razmer + 1);
		Buffer[razmer] = c;
	}
	fclose(Read);

	short flag = 0;
	for (unsigned long long shch = 0; shch < razmer; shch++)
	{
	MarkOutside:
		if (Buffer[shch] == 'i' && Buffer[shch + 1] == 'n' && Buffer[shch + 2] == 't')
		{
			flag++;
			shch += 3;
		}
		else if ((Buffer[shch] == 'c' && Buffer[shch + 1] == 'h' && Buffer[shch + 2] == 'a' && Buffer[shch + 3] == 'r') || (Buffer[shch] == 'v' && Buffer[shch + 1] == 'o' && Buffer[shch + 2] == 'i' && Buffer[shch + 3] == 'd') || (Buffer[shch] == 'F' && Buffer[shch + 1] == 'I' && Buffer[shch + 2] == 'L' && Buffer[shch + 3] == 'E'))
		{
			flag++;
			shch += 4;
		}
		else if (Buffer[shch] == 'f' && Buffer[shch + 1] == 'l' && Buffer[shch + 2] == 'o' && Buffer[shch + 3] == 'a' && Buffer[shch + 4] == 't')
		{
			flag++;
			shch += 5;
		}


		if (flag)
		{
		MarkInside:
			flag = 0;
			i = 0;
			while (isspace(Buffer[shch]) || Buffer[shch] == '*')
			{
				shch++;
				i++;
			}
			if ((isalpha(Buffer[shch]) || Buffer[shch] == '_') && (i != 0))
			{
				while (isalpha(Buffer[shch]) || isdigit(Buffer[shch]) || Buffer[shch] == '_')
				{
					fputc(Buffer[shch], Before);
					shch++;
				}
				fputc('\n', Before);
				LinesCount++;
				while (strchr("(),;{}", Buffer[shch]) == NULL) shch++;

				if (Buffer[shch] == '(') FlagFunction = 1;
				else if (Buffer[shch] == ')') FlagFunction = 0;
				else if (Buffer[shch] == ',' && FlagFunction == 1)
				{
					shch++;
					goto MarkOutside;
				}
				else if (Buffer[shch] == ',' && FlagFunction == 0)
				{
					shch++;
					goto MarkInside;
				}
				else if (Buffer[shch] == '{')
				{
					while (Buffer[shch] != '}')shch++;
					shch++;
				}
			}

		}
	}

	fclose(Before);
	for (int i = 0; i < LinesCount; i++)
	{
		int Length = rand() % 20 + 5;
		for (int j = 0; j < Length; j++)
		{
			unsigned char c;
			if (j % 3 == 0) c = rand() % 25 + 65;
			else if (j % 3 == 1) c = rand() % 25 + 97;
			else c = rand() % 9 + 48;
			fputc(c, After);
		}
		fputc('\n', After);
	}
	fclose(After);
	return LinesCount;
}

void Rewrite(FILE* First, FILE* Second)
{
	int c;
	while ((c = fgetc(First)) != EOF) fputc(c, Second);
	fclose(First);
	fclose(Second);
}

void DeleteCom(FILE* in, FILE* out)
{
	int s1, s2;
	int f;

	while ((s1 = fgetc(in)) != EOF)
	{
	start:
		s2 = '0';
		if (s1 == '\"')
		{
			fputc(s1, out);
			s1 = fgetc(in);
			while (s1 != '\n' && !feof(in) && s1 != '\"')
			{
				fputc(s1, out);
				if (s1 == '\\')
				{
					s1 = fgetc(in);
					if (!feof(in))
					{
						fputc(s1, out);
						s1 = fgetc(in);
					}
				}
				else
					s1 = fgetc(in);
			}
			if (!feof(in))
				fputc(s1, out);
		}
		else if (s1 == '\'')
		{
			fputc(s1, out);
			s1 = fgetc(in);
			while (s1 != '\n' && !feof(in) && s1 != '\'')
			{
				fputc(s1, out);
				if (s1 == '\\')
				{
					s1 = fgetc(in);
					if (!feof(in) && s1 != '\n')
					{
						fputc(s1, out);
						s1 = fgetc(in);
					}
					else goto start;
				}
				else s1 = fgetc(in);
			}
			if (!feof(in)) fputc(s1, out);
		}
		else if (s1 == '/')
		{
			s2 = s1;
			s1 = fgetc(in);
			if (s1 == '/')
			{
				f = 1;
				while (f)
				{
					while (s1 != '\\' && s1 != '\n' && !feof(in))
						s1 = fgetc(in);
					if (s1 == '\\')
					{
						s1 = fgetc(in);
						s1 = fgetc(in);
					}
					else if (s1 == '\n' || feof(in)) goto start;
				}
			}
			else if (s1 == '*')
			{
				s1 = fgetc(in);
				s2 = s1;
				f = 1;
				while (f)
				{
					while (s1 != '*' && !feof(in)) s1 = fgetc(in);
					if (feof(in)) goto start;
					else
					{
						s1 = fgetc(in);
						if (s1 == '/') f = 0;
					}
				}
			}
			else
			{
				fputc(s2, out);
				goto start;
			}
		}
		else if (s1 != EOF)
			fputc(s1, out);
		/*else if (feof(in))
			break;*/
	}
	fclose(in);
	fclose(out);
}

int main() {
	FILE* First;
	FILE* Second;
	FILE* ConfigFile;
	int ConfigData[5];

	srand(time(NULL)); //обновление rand() при каждом запуске программы

	fopen_s(&ConfigFile, "Config.txt", "r");
	for (int i = 0; i < 5; i++) fscanf(ConfigFile, "%d", &ConfigData[i]);//читаем параметры
	fclose(ConfigFile);

	if (ConfigData[0] == 1) // Удаление комментариев
	{
		fopen_s(&First, "Ishodnik.c", "r");
		fopen_s(&Second, "Final.c", "w");
		DeleteCom(First, Second);
	}
	else
	{
		fopen_s(&First, "Ishodnik.c", "r");
		fopen_s(&Second, "Final.c", "w");
		Rewrite(First, Second);
	}

	if (ConfigData[1] == 1) // Замена имен переменных и функций
	{
		fopen_s(&First, "Final.c", "r");
		int Lines = CreateVariables(First);
		RenameVariables(Lines);
	}
	else
	{
		fopen_s(&First, "Final.c", "r");
		fopen_s(&Second, "Draft.c", "w");
		Rewrite(First, Second);
	}

	if (ConfigData[2] == 1) //мусор DONE
	{
		fopen_s(&First, "Draft.c", "r");
		fopen_s(&Second, "Final.c", "w");
		int j = rand() % 10 + 20; //кол-во мусора
		struct CyclesVariables* Generate;
		Generate = (struct CyclesVariables*)malloc(sizeof(struct CyclesVariables) * j); //мусорный массив
		GenerateCycles(j, Generate);
		WriteCycles(First, Second, Generate, j);
		free(Generate);
		Generate = NULL;
	}
	else
	{
		fopen_s(&First, "Draft.c", "r");
		fopen_s(&Second, "Final.c", "w");
		Rewrite(First, Second);
	}

	if (ConfigData[3] == 1) //Перемешивание функций
	{
		int Count;//кол-во функций
		int* Random;

		fopen_s(&First, "Final.c", "r");
		fopen_s(&Second, "Draft.c", "w");
		Count = GenerateFunctionFiles(First, Second);
		Random = (int*)malloc(sizeof(int) * Count);
		memset(Random, -1, sizeof(int) * Count);

		for (int i = 0; i < Count; i++)//Записываем объявление всех функций на каждой строке
		{
			char FunNum[8] = "Fun";
			char Number[3] = { '\0' };
			char c;
			_itoa(i, Number, 10);
			strncat(FunNum, Number, strlen(Number));
			strncat(FunNum, ".c", strlen(".c"));
			fopen_s(&First, "Draft.c", "a");
			fopen_s(&Second, FunNum, "r");
			while ((c = fgetc(Second)) != ')') fputc(c, First);
			fprintf(First, "%c;\n", c);
			fclose(First);
			fclose(Second);
		}

		for (int i = 0; i < Count; i += 2)
		{
			Random[i] = i;
			if (i + 1 < Count) Random[i + 1] = i + 1;
		}

		for (int i = 0; i < Count; i++)
		{
			int j = rand() % Count;
			int k;
			k = Random[i];
			Random[i] = Random[j];
			Random[j] = k;
		}

		for (int i = 0; i < Count; i++)//переписываем сами функции
		{
			char FunNum[8] = "Fun";
			char Number[3] = { '\0' };
			_itoa(Random[i], Number, 10);
			strncat(FunNum, Number, strlen(Number));
			strncat(FunNum, ".c", strlen(".c"));
			fopen_s(&First, "Draft.c", "a");
			fopen_s(&Second, FunNum, "r");
			Rewrite(Second, First);
			remove(FunNum);
		}
		free(Random);
		Random = NULL;
	}
	else
	{
		fopen_s(&First, "Final.c", "r");
		fopen_s(&Second, "Draft.c", "w");
		Rewrite(First, Second);
	}

	if (ConfigData[4] == 1) // Удаление пустых символов
	{
		fopen_s(&First, "Draft.c", "r");
		fopen_s(&Second, "Final.c", "w");
		DeleteSpaces(First, Second);
	}
	else
	{
		fopen_s(&First, "Draft.c", "r");
		fopen_s(&Second, "Final.c", "w");
		Rewrite(First, Second);
	}
	remove("Draft.c");
}