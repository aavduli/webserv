/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 11:22:48 by jim               #+#    #+#             */
/*   Updated: 2025/09/01 11:48:19 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>
#include "config.hpp"

class Location{
	private:
		std::string _path;
		std::vector<ConfParam> _param;

	public:
		Location(const std::string &path = "");
		~Location();

	void addParam(const ConfParam &param);
	std::string getPath() const;
	const std::vector<ConfParam> &getParam() const;

	void print()const;
};

#endif
