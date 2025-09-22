/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingUtils.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 15:25:46 by jim               #+#    #+#             */
/*   Updated: 2025/09/22 15:29:12 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_UTILS_HPP
# define PARSING_UTILS_HPP
# include <string>
# include <vector>

class ParsingUtils{
	public :
		static std::string trim(const std::string& str);
		static std::vector<std::string> split(const std::string& str, char delimiter);
		static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
		static bool isValidIP(const std::string& ip);
		static bool isValidPort(const std::string& port);
		static std::string toLowerCase(const std::string& str);
		static bool isValidMethod(const std::string method);
	};

#endif
