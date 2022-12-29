#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <assert.h>

/* Defining Characters used to denote various chess pieces*/
#define CHESS_PIECE_ROOK '!'
#define CHESS_PIECE_BISHOP '#'
#define CHESS_PIECE_KNIGHT '&'
#define CHESS_PIECE_PAWN '*'
#define CHESS_PIECE_QUEEN '$'
#define CHESS_PIECE_KING  '@'

#define CHESS_NO_PIECE '48'
#define BLANK ' '

/*Defining Colour code to denote coloured square of chessboard*/
#define CHESSBOARD_SQUARE_BLACK 0
#define CHESSBOARD_SQUARE_WHITE 1

/*Defining Colour Codes of Chess pieces*/
#define CHESS_PIECE_BLACK 0
#define CHESS_PIECE_WHITE 1

/*Defining ChessBoard that to be constructed from 2-D Matrix*/
#define CHESSBOARD_RANK 8
#define CHESSBOARD_FILE 8

/*Nomanclaturing FILES of chessboard*/
#define CHESSBOARD_FILE_A 0
#define CHESSBOARD_FILE_B 1
#define CHESSBOARD_FILE_C 2
#define CHESSBOARD_FILE_D 3
#define CHESSBOARD_FILE_E 4
#define CHESSBOARD_FILE_F 5
#define CHESSBOARD_FILE_G 6
#define CHESSBOARD_FILE_H 7

/*Nomanclaturing RANKS of chessboard*/
#define CHESSBOARD_RANK_0 0
#define CHESSBOARD_RANK_1 1
#define CHESSBOARD_RANK_2 2
#define CHESSBOARD_RANK_3 3
#define CHESSBOARD_RANK_4 4
#define CHESSBOARD_RANK_5 5
#define CHESSBOARD_RANK_6 6
#define CHESSBOARD_RANK_7 7
#define CHESSBOARD_RANK_8 8

#define RANK_OFFSET_CORRECTION 1

/*Color code defined for printing on console*/
#define COLOR_CODE_BLACK 4
#define COLOR_CODE_WHITE 2
#define COLOR_CODE_RESET 7
#define RANK_AND_FILE_COLOUR 6

#define STD_MOVE_MEM_SIZE 8

#define SHORT_CASTLE "O-O"
#define LONG_CASTLE "O-O-O"

/*Standard Typedefs*/
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef char chess_move_t;

/*Representing a structure for each square of Chessboard*/
typedef struct chessboard_square
{
    uint8_t chessboard_square_colour;
    uint8_t chess_piece_colour;
    uint8_t chess_piece;
    uint8_t is_first_move_done;
}CHESSBOARD_T;

/*Global Flag Variable*/
uint8_t checkmate_flag = FALSE;
uint8_t draw_flag = FALSE;
uint8_t stalemate_flag = FALSE;
uint8_t valid_move_flag = FALSE;

/*Wrapper Functions for standard library functions*/
void* chessboard_malloc(uint8_t, uint32_t);
void chessboard_free(void*);

/*Wrapper Functions for Win32 APIs*/
HANDLE chessboard_get_std_handle(DWORD);
BOOL chessboard_get_console_screen_buffer_info(HANDLE, CONSOLE_SCREEN_BUFFER_INFO*);
BOOL chessboard_set_console_text_attribute(HANDLE, WORD);

/*Initialiser function for Chessboard array*/
CHESSBOARD_T** initialize_chessboard();

/*Function to print Chessboard status*/
void print_chessboard(CHESSBOARD_T**);

/*Function To print coloured text on console*/
void set_colour(int);

/*Function to setup the chessboard to intial position*/
void initiate_chessboard_setup(CHESSBOARD_T**);

/*helper functions*/
void chess_piece_placement(CHESSBOARD_T**, uint8_t, uint8_t, uint8_t, uint8_t);
void switch_case_for_pieces(CHESSBOARD_T**, uint8_t, uint8_t, uint8_t);

/*Accepting user input for move*/
chess_move_t* player_move_input(void);
uint8_t validate_move(chess_move_t*);
uint8_t chess_input_compare(chess_move_t*, const char*);

int main(void)
{
    CHESSBOARD_T** chessboard_pptr = initialize_chessboard();
    uint8_t invalid_move_count = 0;

    initiate_chessboard_setup(chessboard_pptr);

    print_chessboard(chessboard_pptr);

    while(1)
    {
        chess_move_t* player_white_move = player_move_input();
        

        for(uint8_t count = 0; count < STD_MOVE_MEM_SIZE; count++)
        {
            if(*(player_white_move + count) != '\0')
                printf("%c",*(player_white_move + count));
            else
                break;
        }

        chess_move_t* player_black_move = player_move_input();

        for(uint8_t count = 0; count < STD_MOVE_MEM_SIZE; count++)
        {
            if(*(player_black_move + count) != '\0')
                printf("%c",*(player_black_move + count));
            else
                break;
        }

        draw_flag = TRUE;
        printf("draw_flag : %hhd",draw_flag);

        if(checkmate_flag == TRUE || draw_flag == TRUE || stalemate_flag == TRUE)
            break;
    }
    

    exit(EXIT_SUCCESS);
}

/*Defination of wrapper for malloc function*/
void* chessboard_malloc(uint8_t no_of_mem_blocks, uint32_t size_of_one_mem_block)
{
    void* vptr;
    vptr = malloc(no_of_mem_blocks * size_of_one_mem_block);
    if(vptr == NULL)
    {
        printf("Malloc failed\n");
        exit(EXIT_FAILURE);
    }
    return vptr;
}

/*Defination of wrapper for freeing the memory allocated by malloc*/
void chessboard_free(void *vptr)
{
    free(vptr);
    vptr = NULL;
}

/*Defination of Chessboard array function*/
CHESSBOARD_T** initialize_chessboard()
{
    CHESSBOARD_T** chessboard_pptr;

    chessboard_pptr = (CHESSBOARD_T**)chessboard_malloc(CHESSBOARD_RANK, sizeof(CHESSBOARD_T*));

    for(uint8_t count1 = 0; count1 < CHESSBOARD_FILE; count1++)
    {
        *(chessboard_pptr + count1) = (CHESSBOARD_T*)chessboard_malloc(CHESSBOARD_FILE, sizeof(CHESSBOARD_T));
    }

    for(uint8_t count1 = 0; count1 < CHESSBOARD_RANK; count1++)
    {
        for(uint8_t count2 = 0; count2 < CHESSBOARD_FILE; count2++)
        {
            (*(chessboard_pptr + count1) + count2)->chess_piece = 48;
            (*(chessboard_pptr + count1) + count2)->chess_piece_colour = 3;
        }
    }

    return chessboard_pptr;
}

/*Defination of Chessboard printing function*/
void print_chessboard(CHESSBOARD_T** chessboard_pptr)
{
    uint8_t rank_ref = 9;

    set_colour(COLOR_CODE_BLACK);
    printf("\n\t\t\t\t\t\t\t\tPLAYER 1 (BLACK PIECE PLAYER)\n");
    set_colour(COLOR_CODE_RESET);

    for(uint8_t count1 = 0; count1 < CHESSBOARD_RANK + (2 * RANK_OFFSET_CORRECTION); count1++)
    {

        char file_count = 'a'; 
        printf("\n\t\t\t\t\t\t");
        set_colour(RANK_AND_FILE_COLOUR);

        if(rank_ref == 9 || rank_ref == 0)
            printf("%c",BLANK);
        else    
            printf("\n\n\t\t\t\t\t%d\t",rank_ref);
        
        set_colour(COLOR_CODE_RESET);
        for(uint8_t count2 = 0; count2 < CHESSBOARD_FILE; count2++)
        {
            
            if(count1 == 0 || count1 == 9)
            {
                set_colour(RANK_AND_FILE_COLOUR);
                printf("%c\t",file_count);
                set_colour(COLOR_CODE_RESET);
                file_count++;
            }
            else
            {
                if((*(chessboard_pptr + count1 - RANK_OFFSET_CORRECTION) + count2)->chess_piece_colour == 0)
                {
                    set_colour(COLOR_CODE_WHITE);
                    printf("%c\t",(*(chessboard_pptr + count1 - RANK_OFFSET_CORRECTION) + count2)->chess_piece);
                    set_colour(COLOR_CODE_RESET);
                }
                else if((*(chessboard_pptr + count1 - RANK_OFFSET_CORRECTION) + count2)->chess_piece_colour == 1)
                {
                    set_colour(COLOR_CODE_BLACK);
                    printf("%c\t",(*(chessboard_pptr + count1 - RANK_OFFSET_CORRECTION) + count2)->chess_piece);
                    set_colour(COLOR_CODE_RESET);
                }
                else
                {
                    printf("%c\t",(*(chessboard_pptr + count1 - RANK_OFFSET_CORRECTION) + count2)->chess_piece);
                    set_colour(COLOR_CODE_RESET);
                }
            }
        }

        set_colour(RANK_AND_FILE_COLOUR);

        if(rank_ref == 9 || rank_ref == 0)        
            printf("%c\n\t\t\t",' ');
        else
        {
            printf("%d\n\t\t\t",rank_ref);
            if(rank_ref == 1)
                printf("\n\n");
        }

        set_colour(COLOR_CODE_RESET);
        
        rank_ref--;
    }

    set_colour(COLOR_CODE_WHITE);
    printf("\n\t\t\t\t\t\t\t\tPLAYER 2 (WHITE PIECE PLAYER)\n");
    set_colour(COLOR_CODE_RESET);    
}

/*Defination of Win32 wrapper functions*/
HANDLE chessboard_get_std_handle(DWORD handle_type)
{
    HANDLE output_handle_type = GetStdHandle(handle_type);
    return output_handle_type;
}

BOOL chessboard_get_console_screen_buffer_info(HANDLE handle_type, CONSOLE_SCREEN_BUFFER_INFO* screen_buffer_info)
{
    BOOL output_buffer_info = GetConsoleScreenBufferInfo(handle_type, screen_buffer_info);
    return output_buffer_info;
}

BOOL chessboard_set_console_text_attribute(HANDLE handle_type, WORD font_colour_attributes)
{
    BOOL output_text_attributes = SetConsoleTextAttribute(handle_type, font_colour_attributes);
    return output_text_attributes;
}

void set_colour(int font_colour)
{
     WORD w_colour;
     HANDLE handle_std_out = chessboard_get_std_handle(STD_OUTPUT_HANDLE);
     CONSOLE_SCREEN_BUFFER_INFO csbi;
     if(chessboard_get_console_screen_buffer_info(handle_std_out, &csbi))
     {
          w_colour = (csbi.wAttributes & 0xF0) + (font_colour & 0x0F);
          chessboard_set_console_text_attribute(handle_std_out, w_colour);
     }
     return;
}

void initiate_chessboard_setup(CHESSBOARD_T** chessboard_pptr)
{
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_ROOK, CHESSBOARD_FILE_A, CHESSBOARD_RANK_0, CHESS_PIECE_WHITE);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_KNIGHT, CHESSBOARD_FILE_B, CHESSBOARD_RANK_0, CHESS_PIECE_WHITE);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_BISHOP, CHESSBOARD_FILE_C, CHESSBOARD_RANK_0, CHESS_PIECE_WHITE);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_QUEEN, CHESSBOARD_FILE_D, CHESSBOARD_RANK_0, CHESS_PIECE_WHITE);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_KING, CHESSBOARD_FILE_E, CHESSBOARD_RANK_0, CHESS_PIECE_WHITE);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_BISHOP, CHESSBOARD_FILE_F, CHESSBOARD_RANK_0, CHESS_PIECE_WHITE);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_KNIGHT, CHESSBOARD_FILE_G, CHESSBOARD_RANK_0, CHESS_PIECE_WHITE);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_ROOK, CHESSBOARD_FILE_H, CHESSBOARD_RANK_0, CHESS_PIECE_WHITE);

    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_A, CHESSBOARD_RANK_1, CHESS_PIECE_WHITE);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_B, CHESSBOARD_RANK_1, CHESS_PIECE_WHITE);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_C, CHESSBOARD_RANK_1, CHESS_PIECE_WHITE);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_D, CHESSBOARD_RANK_1, CHESS_PIECE_WHITE);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_E, CHESSBOARD_RANK_1, CHESS_PIECE_WHITE);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_F, CHESSBOARD_RANK_1, CHESS_PIECE_WHITE);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_G, CHESSBOARD_RANK_1, CHESS_PIECE_WHITE);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_H, CHESSBOARD_RANK_1, CHESS_PIECE_WHITE);

    chess_piece_placement(chessboard_pptr, CHESS_PIECE_ROOK, CHESSBOARD_FILE_A, CHESSBOARD_RANK_7, CHESS_PIECE_BLACK);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_KNIGHT, CHESSBOARD_FILE_B, CHESSBOARD_RANK_7, CHESS_PIECE_BLACK);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_BISHOP, CHESSBOARD_FILE_C, CHESSBOARD_RANK_7, CHESS_PIECE_BLACK);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_QUEEN, CHESSBOARD_FILE_D, CHESSBOARD_RANK_7, CHESS_PIECE_BLACK);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_KING, CHESSBOARD_FILE_E, CHESSBOARD_RANK_7, CHESS_PIECE_BLACK);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_BISHOP, CHESSBOARD_FILE_F, CHESSBOARD_RANK_7, CHESS_PIECE_BLACK);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_KNIGHT, CHESSBOARD_FILE_G, CHESSBOARD_RANK_7, CHESS_PIECE_BLACK);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_ROOK, CHESSBOARD_FILE_H, CHESSBOARD_RANK_7, CHESS_PIECE_BLACK);

    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_A, CHESSBOARD_RANK_6, CHESS_PIECE_BLACK);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_B, CHESSBOARD_RANK_6, CHESS_PIECE_BLACK);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_C, CHESSBOARD_RANK_6, CHESS_PIECE_BLACK);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_D, CHESSBOARD_RANK_6, CHESS_PIECE_BLACK);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_E, CHESSBOARD_RANK_6, CHESS_PIECE_BLACK);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_F, CHESSBOARD_RANK_6, CHESS_PIECE_BLACK);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_G, CHESSBOARD_RANK_6, CHESS_PIECE_BLACK);
    chess_piece_placement(chessboard_pptr, CHESS_PIECE_PAWN, CHESSBOARD_FILE_H, CHESSBOARD_RANK_6, CHESS_PIECE_BLACK);


}

void chess_piece_placement(CHESSBOARD_T** chessboard_pptr, uint8_t chess_piece, uint8_t file_name, uint8_t rank_no, uint8_t colour_of_piece)
{
    if(colour_of_piece == CHESS_PIECE_WHITE)
    {
        (*(chessboard_pptr + rank_no) + file_name)->chess_piece_colour = CHESS_PIECE_WHITE;
        switch_case_for_pieces(chessboard_pptr, chess_piece, file_name, rank_no);       
    }
    else
    {
        (*(chessboard_pptr + rank_no) + file_name)->chess_piece_colour = CHESS_PIECE_BLACK;
        switch_case_for_pieces(chessboard_pptr, chess_piece, file_name, rank_no); 
    }
}

void switch_case_for_pieces(CHESSBOARD_T** chessboard_pptr, uint8_t chess_piece, uint8_t file_name, uint8_t rank_no)
{
    switch(chess_piece)
    {
        case CHESS_PIECE_PAWN:
            (*(chessboard_pptr + rank_no) + file_name)->chess_piece = CHESS_PIECE_PAWN;
            break;
            
        case CHESS_PIECE_BISHOP:
            (*(chessboard_pptr + rank_no) + file_name)->chess_piece = CHESS_PIECE_BISHOP;
            break;
            
        case CHESS_PIECE_KNIGHT:
            (*(chessboard_pptr + rank_no) + file_name)->chess_piece = CHESS_PIECE_KNIGHT;
            break;
            
        case CHESS_PIECE_ROOK:
            (*(chessboard_pptr + rank_no) + file_name)->chess_piece = CHESS_PIECE_ROOK;
            break;
            
        case CHESS_PIECE_QUEEN:
            (*(chessboard_pptr + rank_no) + file_name)->chess_piece = CHESS_PIECE_QUEEN;
            break;
            
        case CHESS_PIECE_KING:
            (*(chessboard_pptr + rank_no) + file_name)->chess_piece = CHESS_PIECE_KING;
            break;                
    }
}

chess_move_t* player_move_input(void)
{
    chess_move_t* player_input = NULL;

    player_input = (chess_move_t*)chessboard_malloc(STD_MOVE_MEM_SIZE, sizeof(chess_move_t));

    printf("Enter your Move : ");

    for(uint8_t count = 0; count < STD_MOVE_MEM_SIZE; count++)
    {
        chess_move_t temp_var;
        scanf("%c",&temp_var);

        if(temp_var != '\n')
            *(player_input + count) = temp_var;
        else
        {
            *(player_input + count) = '\0';
            break;
        }
    }
    
    valid_move_flag = validate_move(player_input);
    
    if(valid_move_flag == TRUE)
    {
        return player_input;
    }
    else
    {
        printf("Checking for false flag\n");
        chessboard_free(player_input);
        player_input = NULL;
        player_input = player_move_input();
    }
    
   return player_input;
}

uint8_t validate_move(chess_move_t player_move)
{
    uint8_t valide_move = FALSE;
    uint8_t input_compare = chess_input_compare(player_move, SHORT_CASTLE);
    if (input_compare)
    {
        printf("Calling inoput compare function\n");
        valide_move = TRUE;
        printf("Player played short castle\n");
    }
    return valide_move;
}

uint8_t chess_input_compare(chess_move_t* player_move, const char* std_move)
{
    uint8_t compare_move = FALSE;

    for(uint8_t count = 0; count < STD_MOVE_MEM_SIZE; count++)
    {
        printf("%c/t",*(std_move + count));
        if(*(player_move + count) == *(std_move + count))
            compare_move = TRUE;
   }

    return compare_move;
}