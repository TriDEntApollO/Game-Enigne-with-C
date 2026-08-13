# Game Engine in C with SDL3 and OpenGL

SHELL := powershell.exe
.SHELLFLAGS := -NoProfile -ExecutionPolicy Bypass -Command

define MKDIR_P
    @New-Item -ItemType Directory -Force -Path "$(1)" | Out-Null
endef

define CP
    Copy-Item -Force "$(1)" "$(2)"
endef

define RM_RF
    @if (Test-Path "$(1)") { Remove-Item -Recurse -Force "$(1)" }
endef


PROJECT_NAME := game_engine

CC := cl

SRC_DIR        := src
LIBS_DIR	   := lib
DLL_DIR		   := dll
INCLUDE_DIR    := include

BUILD_DIR      := ./build
BIN_DIR        := $(BUILD_DIR)/bin
OBJ_DIR        := $(BUILD_DIR)/obj

OUTPUT_EXE 	   := $(BIN_DIR)/$(PROJECT_NAME).exe

CPPFLAGS 	   := /I $(INCLUDE_DIR)
CFLAGS 	       := /Zi /Fd"$(OBJ_DIR)/$(PROJECT_NAME)_c.pdb"
LDFLAGS        := /OUT:$(OUTPUT_EXE) /PDB:"$(BIN_DIR)/$(PROJECT_NAME).pdb"
LDLIBS         =  $(LIBS)


rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRCS := $(call rwildcard,$(SRC_DIR),*.c)
LIBS := $(call rwildcard,$(LIBS_DIR),*.lib)
DLLS := $(call rwildcard,$(DLL_DIR),*.dll)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.obj,$(SRCS))

# --- Commands ---
.PHONY: all build run remove clean rebuild 

all: build

build: $(OUTPUT_EXE)
	@echo ""
	@echo "Build completed."

run: $(OUTPUT_EXE)
	@echo "Running the game engine..."
	@$(OUTPUT_EXE)

# Remove build artifacts
remove:
	$(call RM_RF,$(BUILD_DIR)/obj)
	@echo "Removed build artifacts."

# Remove all build artifacts and binaries
clean:
	$(call RM_RF,$(BUILD_DIR))
	@echo "Cleaned all build artifacts and binaries."

rebuild: clean build


# --- Files & Directories ---
$(OUTPUT_EXE): $(OBJ_DIR) $(BIN_DIR) $(OBJS)
	@echo "Linking $(OUTPUT_EXE)..."
	$(CC) $(OBJS) /link $(LDLIBS) $(LDFLAGS)
	@$(foreach dll,$(DLLS),$(call CP,$(dll),$(BIN_DIR));)

$(OBJ_DIR)/%.obj: $(SRC_DIR)/%.c
	$(call MKDIR_P,$(dir $@))
	@echo "Compiling $<..."
	$(CC) /c $(CPPFLAGS) $(CFLAGS) /Fo$@ $<

$(BIN_DIR):
	$(call MKDIR_P,$(BIN_DIR))

$(OBJ_DIR):
	$(call MKDIR_P,$(OBJ_DIR))

