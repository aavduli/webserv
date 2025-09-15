/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_reader.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 10:14:53 by jim               #+#    #+#             */
/*   Updated: 2025/09/15 11:48:39 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_READER_HPP
# define FILE_READER_HPP
# include <string>
# include <vector>
# include <fstream>

class FileReader{
	private:
		std::string trim(const std::string& str) const;

	public :
		std::vector<std::string> readLines(const std::string& filepath) const;
		bool fileExist(const std::string& filepath) const;
};

#endif
