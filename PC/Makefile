CC		:=	gcc
LN		:=	gcc
ODIR	:=	build
SDIR	:=	source
IDIR	:=	include
LDIR	:=	lib
DDIR	:=	dist
CFLAGS	:=	-I$(IDIR) -fms-extensions -O2 -Wall
LFLAGS	:=	$(LDIR)/vJoyInterface.lib -lws2_32 -lGdi32 -lgdiplus -static-libgcc
CFILES	:=	$(wildcard $(SDIR)/*.c)
OBJS	:=	$(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(wildcard $(SDIR)/*.c))
RCFILE	:=	$(SDIR)/3DSController.rc
RESFILE :=  $(ODIR)/3DSController.res

PLATFORM = $(shell uname)

ifeq ($(findstring Linux,$(PLATFORM)),Linux)
	TARGET=3DSController
endif

ifeq ($(findstring Darwin,$(PLATFORM)),Darwin)
	TARGET=3DSController
endif

ifeq ($(findstring MINGW,$(PLATFORM)),MINGW)
	TARGET=3DSController.exe
endif

ifeq ($(findstring MSYS,$(PLATFORM)),MSYS)
	TARGET=3DSController.exe
	gcc=x86_64-w64-mingw32-gcc
	ln=x86_64-w64-mingw32-gcc
endif

$(TARGET): $(ODIR) $(DDIR) $(OBJS) $(RESFILE)
	$(LN) $(ODIR)/*.o -o $(DDIR)/$(TARGET) $(CFLAGS) $(LFLAGS) $(RESFILE)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS) 

$(ODIR):
	@mkdir $@

$(DDIR):
	@mkdir $@

$(RESFILE):
	windres $(RCFILE) -O coff -o $(RESFILE)

.PHONY: clean

clean:
	rm -rf $(TARGET) $(ODIR) $(DDIR)/*.exe
