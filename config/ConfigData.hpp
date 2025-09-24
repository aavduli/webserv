/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_data.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 10:41:41 by jim               #+#    #+#             */
/*   Updated: 2025/09/15 11:59:52 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_DATA_HPP
# define CONFIG_DATA_HPP

# include <map>
# include <string>

struct ServerConfig{
	std::map<std::string, std::string> directives;
	bool isEmpty() const{
		return directives.empty();
	}
};

struct LocationConfig{
	std::string path;
	std::map<std::string, std::string> directives;
};

struct LocationsConfig{
	std::map<std::string, LocationConfig> locations;
};

#endif
