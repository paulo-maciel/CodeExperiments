
VULKAN_SDK_PATH = $(VULKAN_SDK)

CC = g++
CFLAGS = -std=c++17 -I$(VULKAN_SDK_PATH)/include -I/usr/local/include -I.

LDFLAGS = -L/usr/local/lib -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan
LDFLAGS += -lpthread -lm -g -O0

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

myvulkan: MySample.o VulkanApp.o
	g++ $(CFLAGS) -o myvulkan MySample.o VulkanApp.o $(LDFLAGS)

.PHONY: test clean

test: myvulkan
	LD_LIBRARY_PATH=$(VULKAN_SDK_PATH)/lib
	VK_LAYER_PATH=$(VULKAN_SDK_PATH)/etc/vulkan/explicit_layer.d
	./myvulkan

clean:
	rm -f myvulkan  *.o
