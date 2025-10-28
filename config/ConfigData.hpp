/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 10:41:41 by jim               #+#    #+#             */
/*   Updated: 2025/10/28 12:38:58 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_DATA_HPP
# define CONFIG_DATA_HPP

# include <map>
# include <string>
# include <vector>


struct LocationConfig{
	std::string path;
	std::map<std::string, std::string> directives;
};

struct ServerConfig{
	std::map<std::string, std::string> directives;
	std::vector<std::string> listen_ports;
	std::map<std::string, LocationConfig> locations;
	bool isEmpty() const{
		return directives.empty();
	}
};


struct LocationsConfig{
	std::map<std::string, LocationConfig> locations;
};

#endif
