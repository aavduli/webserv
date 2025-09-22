/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileReader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 10:14:53 by jim               #+#    #+#             */
/*   Updated: 2025/09/22 17:27:57 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEREADER_HPP
# define FILEREADER_HPP
# include <string>
# include <vector>
# include <fstream>

class FileReader{
	public :
		std::vector<std::string> readLines(const std::string& filepath) const;
		bool fileExist(const std::string& filepath) const;
};

#endif
