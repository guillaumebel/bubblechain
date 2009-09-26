INCLUDES = \
	-I$(SRCDIR) \
	-Wall \
	$(CLUTTER_CFLAGS) \
	$(CLUTTER_GTK_CFLAGS) \
	$(GTK_CFLAGS) \
	$(GCONF_CFLAGS)
	
bin_PROGRAMS = bubblechain

bubblechain_SOURCES = bubble.c \
											main.c 

               
bubblechain_LDADD = $(GTK_LIBS) \
										$(CLUTTER_LIBS) \
										$(CLUTTER_GTK_LIBS) \
										$(GCONF_LIBS)

