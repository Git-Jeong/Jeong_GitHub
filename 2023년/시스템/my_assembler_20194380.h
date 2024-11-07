/* 
 * my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다. 
 * 
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

typedef unsigned char uchar;

/* 
 * instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
 * 라인 별로 하나의 instruction을 저장한다.
 */
	struct inst_unit
	{
		uchar str[10];
		int ops; //	0:-		1:M		2:R		3:N		4:RR	5:RN
		int format; // 0:not SIC/XE code	1:format 1	2:format 2		3:format 3/4
		uchar op;
	};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;


struct op_unit
{
	uchar str[10];
	int format; // 0:not SIC/XE code	1:format 1	2:format 2		3:format 3/4
	uchar op;
};
typedef struct op_unit opt;
opt* op_table;
int opt_index;

/*
 * 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
 */
static int line_num;

/* 
 * 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
 * operator는 renaming을 허용한다.
 */
struct token_unit
{
	uchar opcode;
	int format;
	int locctr;
	uchar *label;
	uchar *operator;
	uchar operand[MAX_OPERAND][20];
	uchar comment[100];
	int object_code;
};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;
/*
 * 심볼을 관리하는 구조체이다.
 * 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
 * 추후 과제에 사용 예정
 */
struct symbol_unit
{
	uchar symbol[10];
	int section;
	int addr;
};
typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
static int symbol_line;
static int locctr;

struct literal_unit
{
	uchar literal[10];
	int size;
	int addr;
	int LTORG_or_STR_line;
	int section;
};
typedef struct literal_unit liter_table;
liter_table* global_liter_table;

struct section_len_contr{
	int sys_len;
	//이는 섹션별로 프로그램의 길이를 별도로 관리하기 위함.
};
struct section_len_contr sections[100];
int section_line = 0;
int section;


struct hex_binary_unit
{
	int format;
	//format값을 저장하는 변수.
	int op_for_1_2[8];
	//format1, 2용 배열.
	int op_for_3_4[6];
	//format3, 4용 배열.
	int r1[4];
	int r2[4];
	//r1과 r2는 format2전용으로 레지스터의 값을 기입하는 것.
	int nixbpe[6];
	//이러면 흠... 포인터 배열로 만드는게 나으려나?
	int disp_1[12];
	//format 3에서 disp값을 기입하는 변수
	int disp_2[8];
	//format 4에서 메모리가 확장된 만큼의 값을 기입하는 변수
	int ob;
	//object code를 기입하도록 해본 변수 
};
typedef struct hex_binary_unit h_b;
h_b hex_binary_table[MAX_LINES];
static int hex_binary_line;
int pc_count;
//pc를 보관할 변수
//--------------
static uchar *input_file;
static uchar *output_file;
int init_my_assembler(void);
int init_inst_file(uchar *inst_file);
//optable 구현을 위해 inst_file을 불러오는 함수
int init_input_file(uchar *input_file);
//token_table 구현을 위해 input_file을 불러오는 함수
int token_parsing(uchar *str);
//input_file을 토큰화하고 저장하는 함수.
static int assem_pass1(void);
void make_opcode_output(uchar *file_name);
//op table을 계산하고, txt로 출력하는 함수.
void make_symtab_output(uchar* symboltable_txt);
//symbol table을 계산하고, txt로 출력하는 함수.
static int assem_pass2(void);
void make_objectcode_output(uchar *file_name); 
//objectcode를 계산하고, txt파일로 출력하는 함수
void make_final_output(uchar* file_name);
//최종 결과물을 만드는 함수
int find_opcode(const char* str);
//optable에서 str을 통해 opcode를 찾아내는 함수

//
// 
//-------------------------------1
int found_litter_index(uchar str[10]);
//litter table에 있는 operand가 가리키는? 실제 위치의 line을 반환해주는 것.
void literal_table_make(uchar str[20], int line);
//리터럴에 해당되는 문자열을 받아서 리터럴 테이블을 구성하는 함수
int equ_token_control(uchar str[20], uchar token_operator);
//equ에 해당되는 문자열을 받아서 equ 테이블을 구성하는 함수
void locctr_plus(uchar loc_str[20], int loc_token_line);
//locctr값을 계산하여 더해가는 함수
int find_symbol_addr(const char* token, int);
//전달받은 token을 symtab에서 검사하여 addr을 반환하는 함수
int find_format(const uchar loc_str[20]);
//전달받은 str을 optable에서 검사하여 format을 반환하는 함수
int byte_size_check(uchar str[20]);
//전달받은 수의 바이트 사이즈를 확인하는 함수
void make_intermediate(int, int);
//intermediate 파일을 만드는 함수
void xbpe_of_nixbpe(int xbpe[4], int line);
//nixbpe에서 xbpe값에 알맞은 값을 대입시키는 함수
int literal_table_line = 0;
//리터럴 테이블을 구성하는데 있어 총 몇 줄인지 index값을 제어하기 위한 변수.
int literal_count = 0;
	//처음 1회만 사용되도록 하는 것.
int intermediate_count = 0;
	//처음 1회만 사용되도록 하는 것.
void locctr_system_history(int, int, int);
//시스템의 길이를 측정하는데 도움을 주는 함수
int start_locctr = 0;
int operator_START;
int next_operator_index;
int next_operator_index_test = 0;
int equ_control_return_value;
int locctr_history[3] = { 0, 0, 0 };
int system_program_length = 0;
int intermediate_first_loc = 0;
	//일회용 변수임. start문은 일찍 끝나도록 위한 조치
void decimalToBinary(int , int, int);
//---------------------------
//여기는 기능을 테스트 하는 함수들
void program_length(void);
int countDigits(int);
void token_test(void);
void literal_test(void);

