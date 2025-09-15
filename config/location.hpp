/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 11:22:48 by jim               #+#    #+#             */
/*   Updated: 2025/09/13 16:23:18 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

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
