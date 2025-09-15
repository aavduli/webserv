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

def generate_markdown(todos_dict, output_file):
	with open(output_file, 'w', encoding='utf-8') as f:
		f.write("# TODO List\n\n")
		f.write("Liste de tous les TODOs trouvés dans les fichiers .cpp/.hpp\n\n")

		if not todos_dict:
			f.write("Aucun TODO trouvé.\n")
			return

		for filepath, todos in sorted(todos_dict.items()):
			f.write(f"## {filepath}\n\n")
			for line_num, line_content in todos:
				f.write(f"- **Ligne {line_num}**: `{line_content}`\n")
			f.write("\n")

def main():
	parser = argparse.ArgumentParser(description='Extract TODO comments from C++ files')
	parser.add_argument('directory', nargs='?', default='.',
					help='Directory to scan (default: current directory)')
	parser.add_argument('-o', '--output', default='todo.md',
					help='Output file name (default: todo.md)')

	args = parser.parse_args()

	if not os.path.isdir(args.directory):
		print(f"Erreur: {args.directory} n'est pas un répertoire valide")
		return 1

	print(f"Scan du répertoire: {args.directory}")
	todos = scan_directory(args.directory)

	print(f"Génération du fichier: {args.output}")
	generate_markdown(todos, args.output)

	total_todos = sum(len(todo_list) for todo_list in todos.values())
	print(f"Terminé! {total_todos} TODOs trouvés dans {len(todos)} fichiers.")

	return 0

if __name__ == "__main__":
	exit(main())
