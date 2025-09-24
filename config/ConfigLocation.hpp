/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLocation.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 11:22:48 by jim               #+#    #+#             */
/*   Updated: 2025/09/16 13:18:52 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGLOCATION_HPP
#define CONFIGLOCATION_HPP

#include <string>
#include <map>

class Location{
	private:
		std::string _path;
		std::map<std::string, std::string> _params; //key = option value = valeur

	public:
		Location(const std::string &path = "");
		~Location();

	void addParam(const std::string &key, const std::string &value);
	std::string getPath() const;
	std::string getParam(const std::string &key) const;
	const std::map<std::string, std::string> &getAllParams() const;

	void print()const;
};

#endif
