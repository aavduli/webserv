/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingUtils.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 15:25:46 by jim               #+#    #+#             */
/*   Updated: 2025/09/22 16:59:00 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_UTILS_HPP
# define PARSING_UTILS_HPP
# include <string>
# include <vector>

class ParsingUtils{
	public :
		std::string trim(const std::string& str) const;
		std::vector<std::string> split(const std::string& str, char delimiter)const;
		std::vector<std::string> split(const std::string& str, const std::string& delimiter)const;
		bool isValidIP(const std::string& ip)const;
		bool isValidPort(const std::string& port)const;
		std::string toLowerCase(const std::string& str)const;
		bool isValidMethod(const std::string method)const;
		size_t parseSize(const std::string& sizeStr)const;
	};

#endif
