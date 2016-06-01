OBJS = fundlg.obj main.obj settings.obj StaticDialog.obj
RESS = rsrc.res
LD = link
CPPFLAGS = $(CPPFLAGS) /EHsc
LDFLAGS = /subsystem:windows /dll /defaultlib:user32.lib /defaultlib:shell32.lib /defaultlib:kernel32.lib /defaultlib:shlwapi.lib /defaultlib:comctl32.lib

all: fun4c.dll

fun4c.dll: $(OBJS) $(RESS)
	$(LD) $(LDFLAGS) /out:$@ $(OBJS) $(RESS)

clean:
	del *.res *.obj *.exp *.lib *.dll

dist:
	zip fun4c-latest.zip *.ico *.h *.cpp *.mak *.rc Readme.*

dist-bin:
	zip fun4c-bin-latest.zip *.dll Readme.*