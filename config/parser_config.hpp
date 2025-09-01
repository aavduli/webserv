/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_config.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:35:44 by jim               #+#    #+#             */
/*   Updated: 2025/09/01 11:32:07 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_CONFIG_HPP
# define PARSER_CONFIG_HPP

#include <string>
#include <vector>
#include "config.hpp"
#include "location.hpp"

class ParseConfig{
	private:
		std::string trim(const std::string &string) const;

	public :
		ParseConfig();
		~ParseConfig();

		std::vector<ConfParam> parseFile(const std::string &ConfigFile) const;
		std::vector<Location> parseLocation(const std::string &ConfigFile) const;
};

#endif
