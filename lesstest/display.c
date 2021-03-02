#include <termcap.h>
#include "lesstest.h"

extern TermInfo terminfo;
extern int screen_width;
extern int screen_height;

void display_attr(Attr attr) {
	static Attr prev_attr = 0;
	if (attr == prev_attr)
		return;
	if (prev_attr & ATTR_STANDOUT)
		printf("%s", terminfo.exit_standout);
	if (prev_attr & ATTR_BLINK)
		printf("%s", terminfo.exit_blink);
	if (prev_attr & ATTR_BOLD)
		printf("%s", terminfo.exit_bold);
	if (prev_attr & ATTR_UNDERLINE)
		printf("%s", terminfo.exit_underline);
	if (attr & ATTR_UNDERLINE)
		printf("%s", terminfo.enter_underline);
	if (attr & ATTR_BOLD)
		printf("%s", terminfo.enter_bold);
	if (attr & ATTR_BLINK)
		printf("%s", terminfo.enter_blink);
	if (attr & ATTR_STANDOUT)
		printf("%s", terminfo.enter_standout);
	prev_attr = attr;
}

void display_color(Color fg_color, Color bg_color) {
printf("{%x/%x}", fg_color, bg_color);
}

void display_screen(const char1* img, int imglen, int move_cursor) {
	int x = 0;
	int y = 0;
	int cursor_x = 0;
	int cursor_y = 0;
	while (imglen-- > 0) {
		wchar ch = load_wchar(&img);
		if (ch == '\\') {
			ch = *img++;
		} else if (ch == '@') {
			Attr attr = *img++;
			display_attr(attr);
			continue;
		} else if (ch == '$') {
			Color fg_color = *img++;
			Color bg_color = *img++;
			display_color(fg_color, bg_color);
			continue;
		} else if (ch == '#') {
			cursor_x = x;
			cursor_y = y;
			continue;
		}
		char1 cbuf[4];
		char1* p = cbuf;
		store_wchar(&p, ch);
		fwrite(cbuf, 1, p-cbuf, stdout);
		if (++x >= screen_width) {
			printf("\n");
			x = 0;
			if (++y >= screen_height)
				break;
		}
	}
	if (move_cursor)
		printf("%s", tgoto(terminfo.cursor_move, cursor_x, cursor_y)); //FIXME
}

void print_strings(const char* title, char* const* strings) {
	fprintf(stderr, "%s:\n", title);
	char* const* s;
	for (s = strings; *s != NULL; ++s) {
		fprintf(stderr, " ");
		const char* p;
		for (p = *s; *p != '\0'; ++p) {
			if (*p >= 0x20 && *p < 0x7f)
				fprintf(stderr, "%c", (char) *p);
			else
				fprintf(stderr, "\\x%x", *p);
		}
		fprintf(stderr, "\n");
	}
}
