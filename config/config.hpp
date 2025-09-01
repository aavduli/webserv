/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 10:26:40 by jim               #+#    #+#             */
/*   Updated: 2025/09/01 10:39:51 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

class ConfParam{
	private :
		std::string _param;
		std::string _comment;

	public:
		ConfParam(const std::string &param = "", const std::string &comment = "");
		~ConfParam();

		std::string getParam() const;
		std::string getComment() const;

		void 		print() const;
};


#endif
