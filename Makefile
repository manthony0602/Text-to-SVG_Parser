all:
	g++ main.cpp -o text_to_svg

run: all
	./text_to_svg
