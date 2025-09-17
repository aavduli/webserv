#!/usr/bin/env python3
import os
import re
import argparse

def find_todos_in_file(filepath):
	todos = []
	try:
		with open(filepath, 'r', encoding='utf-8', errors='ignore') as file:
			for line_num, line in enumerate(file, 1):
				if 'TODO' in line.upper():
					todos.append((line_num, line.strip()))
	except Exception as e:
		print(f"Erreur lors de la lecture de {filepath}: {e}")
	return todos

def scan_directory(directory):
	all_todos = {}

	for root, dirs, files in os.walk(directory):
		for file in files:
			if file.endswith(('.cpp', '.hpp', '.c', '.h')):
				filepath = os.path.join(root, file)
				todos = find_todos_in_file(filepath)
				if todos:
					all_todos[filepath] = todos

	return all_todos

def update_readme_with_todos(todos_dict, readme_path):
	# Lire le contenu existant du README ou créer un nouveau contenu
	existing_content = ""
	if os.path.exists(readme_path):
		try:
			with open(readme_path, 'r', encoding='utf-8') as f:
				existing_content = f.read()
		except Exception as e:
			print(f"Erreur lors de la lecture de {readme_path}: {e}")
	
	# Supprimer la section TODO existante si elle existe
	todo_start_pattern = r'\n## TODO List\n'
	todo_end_pattern = r'\n(?=##|\Z)'
	
	# Rechercher et supprimer la section TODO existante
	todo_match = re.search(todo_start_pattern + r'.*?(?=' + todo_end_pattern + ')', existing_content, re.DOTALL)
	if todo_match:
		existing_content = existing_content[:todo_match.start()] + existing_content[todo_match.end():]
	
	# Générer la nouvelle section TODO
	todo_section = "\n## TODO List\n\n"
	
	if not todos_dict:
		todo_section += "Aucun TODO trouvé dans les fichiers .cpp/.hpp\n"
	else:
		todo_section += "Liste de tous les TODOs trouvés dans les fichiers .cpp/.hpp\n\n"
		for filepath, todos in sorted(todos_dict.items()):
			todo_section += f"### {filepath}\n\n"
			for line_num, line_content in todos:
				todo_section += f"- **Ligne {line_num}**: `{line_content}`\n"
			todo_section += "\n"
	
	# Ajouter la nouvelle section TODO à la fin du fichier
	updated_content = existing_content.rstrip() + todo_section
	
	# Écrire le contenu mis à jour
	try:
		with open(readme_path, 'w', encoding='utf-8') as f:
			f.write(updated_content)
	except Exception as e:
		print(f"Erreur lors de l'écriture de {readme_path}: {e}")
		return False
	
	return True

def main():
	parser = argparse.ArgumentParser(description='Extract TODO comments from C++ files and update README.md')
	parser.add_argument('directory', nargs='?', default='.',
					help='Directory to scan (default: current directory)')

	args = parser.parse_args()

	if not os.path.isdir(args.directory):
		print(f"Erreur: {args.directory} n'est pas un répertoire valide")
		return 1

	print(f"Scan du répertoire: {args.directory}")
	todos = scan_directory(args.directory)

	# Le README.md se trouve dans le même répertoire que le script
	script_dir = os.path.dirname(os.path.abspath(__file__))
	readme_path = os.path.join(script_dir, 'README.md')
	
	print(f"Mise à jour du README.md: {readme_path}")
	success = update_readme_with_todos(todos, readme_path)
	
	if success:
		total_todos = sum(len(todo_list) for todo_list in todos.values())
		print(f"Terminé! {total_todos} TODOs trouvés dans {len(todos)} fichiers.")
		print(f"README.md mis à jour avec succès.")
	else:
		print("Erreur lors de la mise à jour du README.md")
		return 1

	return 0

if __name__ == "__main__":
	exit(main())
