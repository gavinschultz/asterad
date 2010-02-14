OBJ = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.obj,$(wildcard $(SRCDIR)/*.c)) 
DEPS = $(patsubst $(SRCDIR)/%.c,$(INCDIR)/%.h,$(wildcard $(INCDIR)/*.h)) 
OBJDIR = .
INCDIR = include
SRCDIR = src
OUTPUTDIR = .
RESDIR = resources
LIBDIR = lib
CC = cl.exe
MT = mt.exe
LINK = link.exe
CFLAGS = /D "_CRT_SECURE_NO_WARNINGS" /MT /O2 /TC /c -I$(INCDIR) /W3
LIBS = SDL.lib SDLmain.lib SDL_mixer.lib glu32.lib opengl32.lib
DEL = del /F /Q
COPY = copy /Y

asterad: $(OBJ)
	$(LINK) /INCREMENTAL /SUBSYSTEM:WINDOWS /MACHINE:X86 /OUT:"$(OUTPUTDIR)\$@.exe" $(OBJ) $(LIBS)
	 
$(OBJDIR)/%.obj : $(SRCDIR)/%.c $(DEPS) 
	$(CC) $(CFLAGS) $(CPPFLAGS) $<

install: asterad
	$(COPY) $(LIBDIR)\*.* $(OUTPUTDIR)\ 
#	$(MT) /manifest $(OUTPUTDIR)\asterad.exe.manifest /outputresource:$(OUTPUTDIR)\asterad.exe;1
#	$(DEL) $(OUTPUTDIR)\*.manifest
	$(DEL) $(OUTPUTDIR)\*.ilk
	$(DEL) $(OUTPUTDIR)\*.obj
	
clean:
	$(DEL) $(OBJDIR)\*.obj
	$(DEL) $(OUTPUTDIR)\*.exe
	$(DEL) $(OUTPUTDIR)\*.manifest
	$(DEL) $(OUTPUTDIR)\*.ilk
