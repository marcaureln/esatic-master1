APP_NAME = parking
BUILD_DIR = bin
SRC_DIR = source

all: clean build run

build:
	@echo "⏳ Compilation de l'application..."
	@echo "🔍 Vérification des dépendances..."
	@if !(mysql_config --libs > /dev/null); then echo "Veuillez installer les librairies MySQL"; exit 1; fi
	@mkdir -p $(BUILD_DIR)
	@echo "🔧 Compilation des fichiers sources..."
	@gcc $(SRC_DIR)/*.c $$(mysql_config --libs) -o $(BUILD_DIR)/$(APP_NAME)
	@echo "📦 Compilation terminée.\n"

run:
	@$(BUILD_DIR)/$(APP_NAME)

clean:
	@rm -rf ./*/*.o
	@rm -rf ./*/*.out
	@rm -rf $(BUILD_DIR)
