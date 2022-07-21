CFLAGS = -std=c++17 -g -O0 # -O2
LDFLAGS = -lglfw -lvulkan

HelloVulkan: *.cpp *.hpp
	clang++ $(CFLAGS) -o HelloVulkan *.cpp $(LDFLAGS)

.PHONY: test, clean

test: HelloVulkan
	./HelloVulkan

clean:
	rm -f HelloVulkan

