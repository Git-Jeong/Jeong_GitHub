/* 
 * my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�. 
 * 
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

typedef unsigned char uchar;

/* 
 * instruction ��� ���Ϸ� ���� ������ �޾ƿͼ� �����ϴ� ����ü �����̴�.
 * ���� ���� �ϳ��� instruction�� �����Ѵ�.
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
 * ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
 */
static int line_num;

/* 
 * ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
 * operator�� renaming�� ����Ѵ�.
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
 * �ɺ��� �����ϴ� ����ü�̴�.
 * �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
 * ���� ������ ��� ����
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
	//�̴� ���Ǻ��� ���α׷��� ���̸� ������ �����ϱ� ����.
};
struct section_len_contr sections[100];
int section_line = 0;
int section;


struct hex_binary_unit
{
	int format;
	//format���� �����ϴ� ����.
	int op_for_1_2[8];
	//format1, 2�� �迭.
	int op_for_3_4[6];
	//format3, 4�� �迭.
	int r1[4];
	int r2[4];
	//r1�� r2�� format2�������� ���������� ���� �����ϴ� ��.
	int nixbpe[6];
	//�̷��� ��... ������ �迭�� ����°� ��������?
	int disp_1[12];
	//format 3���� disp���� �����ϴ� ����
	int disp_2[8];
	//format 4���� �޸𸮰� Ȯ��� ��ŭ�� ���� �����ϴ� ����
	int ob;
	//object code�� �����ϵ��� �غ� ���� 
};
typedef struct hex_binary_unit h_b;
h_b hex_binary_table[MAX_LINES];
static int hex_binary_line;
int pc_count;
//pc�� ������ ����
//--------------
static uchar *input_file;
static uchar *output_file;
int init_my_assembler(void);
int init_inst_file(uchar *inst_file);
//optable ������ ���� inst_file�� �ҷ����� �Լ�
int init_input_file(uchar *input_file);
//token_table ������ ���� input_file�� �ҷ����� �Լ�
int token_parsing(uchar *str);
//input_file�� ��ūȭ�ϰ� �����ϴ� �Լ�.
static int assem_pass1(void);
void make_opcode_output(uchar *file_name);
//op table�� ����ϰ�, txt�� ����ϴ� �Լ�.
void make_symtab_output(uchar* symboltable_txt);
//symbol table�� ����ϰ�, txt�� ����ϴ� �Լ�.
static int assem_pass2(void);
void make_objectcode_output(uchar *file_name); 
//objectcode�� ����ϰ�, txt���Ϸ� ����ϴ� �Լ�
void make_final_output(uchar* file_name);
//���� ������� ����� �Լ�
int find_opcode(const char* str);
//optable���� str�� ���� opcode�� ã�Ƴ��� �Լ�

//
// 
//-------------------------------1
int found_litter_index(uchar str[10]);
//litter table�� �ִ� operand�� ����Ű��? ���� ��ġ�� line�� ��ȯ���ִ� ��.
void literal_table_make(uchar str[20], int line);
//���ͷ��� �ش�Ǵ� ���ڿ��� �޾Ƽ� ���ͷ� ���̺��� �����ϴ� �Լ�
int equ_token_control(uchar str[20], uchar token_operator);
//equ�� �ش�Ǵ� ���ڿ��� �޾Ƽ� equ ���̺��� �����ϴ� �Լ�
void locctr_plus(uchar loc_str[20], int loc_token_line);
//locctr���� ����Ͽ� ���ذ��� �Լ�
int find_symbol_addr(const char* token, int);
//���޹��� token�� symtab���� �˻��Ͽ� addr�� ��ȯ�ϴ� �Լ�
int find_format(const uchar loc_str[20]);
//���޹��� str�� optable���� �˻��Ͽ� format�� ��ȯ�ϴ� �Լ�
int byte_size_check(uchar str[20]);
//���޹��� ���� ����Ʈ ����� Ȯ���ϴ� �Լ�
void make_intermediate(int, int);
//intermediate ������ ����� �Լ�
void xbpe_of_nixbpe(int xbpe[4], int line);
//nixbpe���� xbpe���� �˸��� ���� ���Խ�Ű�� �Լ�
int literal_table_line = 0;
//���ͷ� ���̺��� �����ϴµ� �־� �� �� ������ index���� �����ϱ� ���� ����.
int literal_count = 0;
	//ó�� 1ȸ�� ���ǵ��� �ϴ� ��.
int intermediate_count = 0;
	//ó�� 1ȸ�� ���ǵ��� �ϴ� ��.
void locctr_system_history(int, int, int);
//�ý����� ���̸� �����ϴµ� ������ �ִ� �Լ�
int start_locctr = 0;
int operator_START;
int next_operator_index;
int next_operator_index_test = 0;
int equ_control_return_value;
int locctr_history[3] = { 0, 0, 0 };
int system_program_length = 0;
int intermediate_first_loc = 0;
	//��ȸ�� ������. start���� ���� �������� ���� ��ġ
void decimalToBinary(int , int, int);
//---------------------------
//����� ����� �׽�Ʈ �ϴ� �Լ���
void program_length(void);
int countDigits(int);
void token_test(void);
void literal_test(void);

