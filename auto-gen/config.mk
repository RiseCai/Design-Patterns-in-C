REPLACE_ME= XXXXXX
OUTDIR    = $(REPLACE_ME)
#_______________________________________________________________________________
#                       PROJECTS GLOBAL CONFIG                      IGNORE ABOVE
#VERBOSE   = 1
DEBUGMODE = 1
G_OUTDIR_DEBUG   = out_debug
G_OUTDIR_RELEASE = out_release
G_OUTDIR_PROFILE = out_profile

G_MACROS  =
G_INCS    = $(TOPDIR)util .
G_LIBDIRS = $(TOPDIR)$(G_OUTDIR_DEBUG)/util
G_LIBS    = util
G_CPPFLAGS= -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wunused-value -Wno-unused-parameter -Wformat \
  -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Wredundant-decls
#-Werror
G_LDFLAGS =
G_ARFLAGS = -cr

#_______________________________________________________________________________
#                                                                   IGNORE BELOW
include $(TOPDIR)base.mk
