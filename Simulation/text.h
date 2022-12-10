#ifndef TEXT_H_INCLUDED
#define TEXT_H_INCLUDED

//will get the first n ascii chars from bitmap
#define ASCII_MAX 128

struct text_character {
	unsigned int texture_id;
	float size[2];
	float bearing[2];
	unsigned int advance;
};

struct text_character text_character_set[ASCII_MAX];


void initialize_text_character_set();


void render_text(unsigned int shader_program, char* text, float x, float y, float scale, float color[3], float* out_width, float* out_height);

#endif
