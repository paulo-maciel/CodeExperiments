
VULKAN_SDK_PATH = $(VULKAN_SDK)

CXX = g++
#CXX = clang
CXXFLAGS = -std=c++17 -I$(VULKAN_SDK_PATH)/include -I/usr/local/include -I.  -g -O0 -ggdb

LDFLAGS = -L/usr/local/lib -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan
LDFLAGS += -lpthread -lm

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

OBJS = MySample.o VulkanApp.o Device.o QueueSelector.o SwapChain.o GraphicsPipeline.o \
       CommandPool.o SyncObjects.o VertexBuffer.o Buffer.o UniformBuffer.o DescriptorPool.o \
			 Image.o TextureImage.o TextureImageSampler.o DepthStencil.o \

myvulkan: $(OBJS)
	$(CXX) $(CXXFLAGS) -o myvulkan $(OBJS) $(LDFLAGS)

.PHONY: test clean

test: myvulkan
	LD_LIBRARY_PATH=$(VULKAN_SDK_PATH)/lib
	VK_LAYER_PATH=$(VULKAN_SDK_PATH)/etc/vulkan/explicit_layer.d
	./myvulkan

clean:
	rm -f myvulkan  *.o
