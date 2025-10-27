/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:35:44 by jim               #+#    #+#             */
/*   Updated: 2025/09/15 13:31:09 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# include <string>
# include <vector>
# include <map>
# include <sys/stat.h>

# include "ConfigData.hpp"

class ConfigParser{
	private:
		static const std::string SERVER_START;
		static const std::string LOCATION_START;
		static const std::string BLOCK_END;

		enum ParseState{
			OUTSIDE_BLOCK,
			IN_SERVER_BLOCK,
			IN_LOCATION_BLOCK,
		};

		std::pair<std::string, std::string> parseDirective(const std::string& line) const;
		void skipBlock(size_t& index, const std::vector<std::string>& lines) const;

		public:
		static std::string normalizeRootPath(const std::string& path);
		ServerConfig parseServer(const std::vector<std::string>& lines) const;
		LocationsConfig parseLocations(const std::vector<std::string>& lines) const;
		std::vector<ServerConfig> parseAllServers(const std::vector<std::string>& lines) const;
};
#endif
