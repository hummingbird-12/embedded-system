#ifndef _DRIVER_H_INCLUDED_
#define _DRIVER_H_INCLUDED_

#define SUCCESS 0

#define STATUS_EXIT -1
#define STATUS_INPUT 0
#define STATUS_GUESSED 1

#define MAX_LIVES 8
#define WORDS_CNT 30
#define WORD_MAX_LEN 20
#define BONUS_TIME_PER_CHAR 5

void sleep_app(void);
void wake_app(void);
void game_set_selected_letter(const char);
void game_make_guess(void);
void game_skip_word(void);
void game_expire_bonus(void);
void game_exit(void);

#endif /* _DRIVER_H_INCLUDED_ */
