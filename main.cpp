// 궁극적인 목표 (v3.0.0): 바이너리 파일 암호화
// 2016/01/27 오후 06:47 아직 v1.0.0에서 건드린 것 없음
// 2016/01/27 오후 08:14 v2.0.0 alpha 완성 (원문, Key 콘솔 입력, 암호문 encrypted.txt 출력)
// 2016/01/27 오후 10:42 v2.0.0 beta 완성 (원문 original.txt 입력, Key 콘솔 입력, 암호문 encrypted.txt 출력)
// 2016/01/28 오전 12:15 v2.0.0 beta 2 완성 (random_key()에서 key % 255 -> key % 128로 수정)
//											(= char형에 음수로 저장되어 파일에 제대로 저장되지 않는 버그 수정)
// 2016/01/28 오전 12:50 v2.0.0 beta 3 완성 (random_key()에서 key /= 16 추가)
//											(= key가 무조건 HEX 값으로 끝에 0이 붙는(무조건 16의 배수인),
//												결과적으로 key가 랜덤하지 않은 버그 수정)
// 2016/01/31 오전 12:00 v2.0.0 Release 완성 (거의 모든 버그 수정)
// 2016/02/06 오후 04:00 v2.0.0 Release 1 완성
//											(1. original.txt 중간에 NULL이 들어 있으면 NULL 뒤로 잘리는 버그 수정,
//											 2. 한글 / 영어 선택할 수 있게 수정 (언어 설정 추가)
//											 3. key.txt가 존재할 경우.txt에서 입력받고 존재하지 않을 경우 표준 입력으로 받게 수정)
// 2016/02/07 오후 02:35 v2.0.0 Release 2 완성 (언어 설정을 cmd에서도 할 수 있게 수정)
// 2016/02/07 오후 04:00 v2.0.0 Release 3 완성 (key.txt가 0byte이거나 전부 NULL일 때 Crash​ (작동이 중지되었습니다) 수정)
// 2016/02/26 오후 09:46 v2.0.0 Release 4 완성
//											(1. fprintf()로 인한 개행 문자 관련 버그 fwrite(,1,1,) 로 고침
//											 2. cls() 없애는 `/noclear` 명령어 추가
//											 3. 'system("pause") 용도로 쓰는 getch()'를 없애는 `/nopause` 명령어 추가)
// 2016/02/29 오후 11:30 v2.1.0 완성
//											(1. access()를 이용한 original.txt 및 key.txt 존재 여부 확인
//											 2. 프로그램 이름 const char[]에 저장)
#include "ustd v4.5.6.h"
​
void Title();  // 제목 출력
void ParameterSettings(int argc, char *argv[]);  // 파라미터 및 언어 설정
​
void Encrypt(char* dest, char* enc);  // 암호화 (XEA 알고리즘 사용)
void Decrypt(char* enc, char* dec);  // 복호화 (XEA 알고리즘 사용)
​
char random_key(int seed);  // 거의 완벽에 가깝게 random하게 key를 제작하도록 설계된 알고리즘.
​
char password[4096];
char original[4096 + 5], encrypted[4096 + 5], decrypted[4096 + 5];
int original_len;  // 원문의 길이를 저장 (암호문의 길이와 복호문의 길이와 일치)
​
const char version[5][10] = { "v2.1.0", "", "" };
const int ver_color[5] = { D_GRAY, D_SKY, D_VIOLET };
​
int lang;  // 1: 한국어, 2: 영어
bool nopause = false; // true: nopause, false: pause
bool noclear = false; // true: noclear, false: clear
​
void main(int argc, char *argv[])
{
	// 언어 설정
	ParameterSettings(argc, argv);
	​
		Title();
	​
		// 프로그램 메인 화면으로 전환
		if (!noclear)
		{
			cls();
			Title();
		}
	​
		​
		// 원문 입력 (파일)
		if (access("original.txt", 0) == -1)  // original.txt가 존재하지 않는다면 (access() 모드 0: 파일 존재 여부 확인. 존재하면 0, 존재하지 않으면 -1 반환)
		{
			gotoxy(0, 2);
			_RED;
			cout << "ERROR: original.txt가 존재하지 않으므로 암호화 프로그램을 시작할 수 없습니다." << endl;
			Delay(2);
			​
				_GRAY;
			return;
		}
		else
		{
			FILE* read_original = fopen("original.txt", "rb");
			original_len = fread(original, 1, 4096, read_original);  // 원문: 바이너리 모드로 최대 4096byte까지 입력받음
			​
				fclose(read_original);
		}
	​
		​
		// 비밀번호(key) 입력 (파일 or 콘솔)
		FILE* read_key = fopen("key.txt", "rb");
	if (read_key != 0)  // key.txt가 존재한다면 key.txt에서 입력받음
	{
		fread(password, 1, 4096, read_key);
		fclose(read_key);
	}
	else if (read_key == 0 || strlen(password) == 0)  // key.txt가 존재하지 않거나 password가 비어 있거나 NULL로 채워져 있다면 (strlen(password)가 0이 되면) 콘솔로 입력받음
	{
		do
		{
			if (!noclear)
			{
				cls();
				Title();
				gotoxy(0, 2);
			}
			_GREEN;
			if (lang == 1)  // 한국어
			{
				cout << endl << "[ 비밀번호 입력 ]" << endl;
			}
			else  // 영어
			{
				cout << endl << "[ Input Password ]" << endl;
			}
			cin.getline(password, 4096);  // 비밀번호: 공백 포함하여 최대 4096 byte까지 입력받음
			​
		} while (strlen(password) == 0);  // Enter 등을 눌러 비밀번호를 입력하지 않았다면 암호화 및 복호화를 할 수 없기 때문에 (strlen(password)으로 나누는 과정에서 ERROR) 다시 입력받는다.
	}
	​
		​
		// 암호화 결과 출력 화면으로 전환
		if (!noclear)
		{
			cls();
			gotoxy(0, 0);
			Title();
		}
	​
		​
		// 원문 출력
		_SKY;
	if (lang == 1)  // 한국어
	{
		cout << endl << endl << endl << "[ 원문 ]" << endl;
	}
	else  // 영어
	{
		cout << endl << endl << endl << "[ Original Text ]" << endl;
	}
	for (int i = 0; i < original_len; i++)  // NULL 때문에 반드시 한 글자씩 출력해야 함.
	{
		cout << original[i];
	}
	​
		​
		// 암호화
		Encrypt(original, encrypted);
	​
		​
		// 암호문 출력
		_GREEN;
	cout << endl << endl << endl;
	if (lang == 1)  // 한국어
	{
		cout << "[ 암호문 ]" << endl;
	}
	else  // 영어
	{
		cout << "[ Encrypted Text ]" << endl;
	}
	for (int i = 0; i < original_len; i++)  // NULL 때문에 반드시 한 글자씩 출력해야 함.
	{
		cout << encrypted[i];
	}
	​
		​
		// 복호화
		Decrypt(encrypted, decrypted);
	​
		​
		// 복호문 출력
		_D_SKY;
	cout << endl << endl << endl;
	if (lang == 1)  // 한국어
	{
		cout << "[ 복호문 ]" << endl;
	}
	else  // 영어
	{
		cout << "[ Decrypted Text ]" << endl;
	}
	​
		for (int i = 0; i < original_len; i++)  // NULL 때문에 반드시 한 글자씩 출력해야 함.
		{
			cout << decrypted[i];
		}
	​
		​
		// 파일에도 암호문 출력
		FILE *w = fopen("encrypted.txt", "w");
	​
		for (int i = 0; i < original_len; i++)  // NULL 때문에 반드시 한 글자씩 출력해야 함.
		{
			fwrite(encrypted + i, 1, 1, w);  // fprintf()를 쓰지 않은 이유: fprintf()는 `\n`을 `\r`,`\n`으로 저절로 바꿔버려 암/복호화에 문제가 생긴다.
		}
	​
		fclose(w);
	​
		​
		// 종료
		_GRAY;
	if (lang == 1)  // 한국어
	{
		cout << endl << endl << endl << "계속하려면 아무 키나 누르십시오 . . ." << endl;
	}
	else  // 영어
	{
		cout << endl << endl << endl << "Press any key to continue..." << endl;
	}
	if (!nopause)
	{
		system("pause");
	}
}
​
void Title()
{
	if (!noclear)
	{
		gotoxy(0, 0);
	}
	_GRAY;
	cout << "E.N.C.R.Y.P.T. ";
	​
		for (int i = 0; i < 3; i++)
		{
			Color(ver_color[i]);
			cout << version[i] << " ";
		}
	​
		if (!noclear)
		{
			gotoxy(100 - strlen("Copyrightⓒ 2016 Name10. All Rights Reserved. "), 0);
			_GOLD;
			cout << "Copyrightⓒ 2016 Name10. All Rights Reserved.";
		}
}
​
void ParameterSettings(int argc, char *argv[])
{
	if (argc == 1)  // cmd에서 파라미터 입력을 하지 않았다면 프로그램에서 언어만 설정
	{
		ConsoleSize(100, 32);
		​
			do
			{
				cls();
				Title();
				​
					_GRAY;
				cout << endl << endl << "Parameter Settings" << endl << endl;
				cout << "\t<1> Korean (한국어)" << endl;
				cout << "\t<2> English" << endl << endl;
				​
					cout << "Choose: ";
				buf_clear();  // Buffer Clear
				lang = getche() - '0';  // 언어 설정
				​
					Delay(0.3);
			} while (lang < 1 || lang > 2);  // 제대로 입력될 때까지 반복
	}
	return;
	​
		// cmd에서 파라미터 입력을 했다면 파라미터에서 설정 추출
		for (int i = 1; i <= argc - 1; i++)
		{
			if (strcmp(argv[i], "/en") == 0 || strcmp(argv[i], "/eng") == 0)
			{
				lang = 1;
			}
			else if (strcmp(argv[i], "/ko") == 0 || strcmp(argv[i], "/kor") == 0)
			{
				lang = 2;
			}
			else if (strcmp(argv[i], "/noclear") == 0)
			{
				noclear = true;
			}
			else if (strcmp(argv[i], "/nopause") == 0)
			{
				nopause = true;
			}
		}
	​
		if (!noclear)
		{
			ConsoleSize(100, 32); // 화면을 클리어 하지 않는다면 이걸 바꿀 이유도 없다
		}
	​
}
​
void Encrypt(char* dest, char* enc)
{
	for (int i = 0; i < original_len; i++)
	{
		// strlen(password)가 0일 경우 0으로 나누기 때문에 ERROR (Integer division by zero.)
		// 따라서 절대로 password가 비어 있거나 NULL로 채워져 있는 일이 없어야 한다.
		enc[i] = dest[i] ^ random_key(i) ^ password[i % strlen(password)];
	}
}
​
void Decrypt(char* enc, char* dec)
{
	for (int i = 0; i < original_len; i++)
	{
		// strlen(password)가 0일 경우 0으로 나누기 때문에 ERROR (Integer division by zero.)
		// 따라서 절대로 password가 비어 있거나 NULL로 채워져 있는 일이 없어야 한다.
		dec[i] = enc[i] ^ random_key(i) ^ password[i % strlen(password)];
	}
}
​
char random_key(int seed)
{
	// 뷁
}
