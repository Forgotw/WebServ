/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 18:51:50 by lsohler           #+#    #+#             */
/*   Updated: 2024/04/08 14:05:16 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/*
Berners-Lee, et al.         Standards Track                    [Page 12]
RFC 3986                   URI Generic Syntax               January 2005


      reserved    = gen-delims / sub-delims

      gen-delims  = ":" / "/" / "?" / "#" / "[" / "]" / "@"

      sub-delims  = "!" / "$" / "&" / "'" / "(" / ")"
                  / "*" / "+" / "," / ";" / "="


2.3.  Unreserved Characters

   Characters that are allowed in a URI but do not have a reserved
   purpose are called unreserved.  These include uppercase and lowercase
   letters, decimal digits, hyphen, period, underscore, and tilde.

      unreserved  = ALPHA / DIGIT / "-" / "." / "_" / "~"


3.  Syntax Components

   The generic URI syntax consists of a hierarchical sequence of
   components referred to as the scheme, authority, path, query, and
   fragment.

      URI         = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

      hier-part   = "//" authority path-abempty
                  / path-absolute
                  / path-rootless
                  / path-empty

   The scheme and path components are required, though the path may be
   empty (no characters).  When authority is present, the path must
   either be empty or begin with a slash ("/") character.  When
   authority is not present, the path cannot begin with two slash
   characters ("//").  These restrictions result in five different ABNF
   rules for a path (Section 3.3), only one of which will match any
   given URI reference.

   The following are two example URIs and their component parts:

         foo://example.com:8042/over/there?name=ferret#nose
         \_/   \______________/\_________/ \_________/ \__/
          |           |            |            |        |
       scheme     authority       path        query   fragment
*/

static const std::string methods_syntax[] = {"GET", "POST", "DELETE"};

static const std::string gen_delims = ":/?#[]@";

static const std::string sub_delims = "!$&'()*+,;=";

static const std::string unreserved[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", "abcdefghijklmnopqrstuvwxyz", "0123456789", "-._~"};

bool is_gen_delim(char character) {
	return gen_delims.find(character) != std::string::npos;
}

bool is_sub_delim(char character) {
	return sub_delims.find(character) != std::string::npos;
}

bool is_unreserved(char character) {
	for (int i = 0; i < sizeof(unreserved) / sizeof(unreserved[0]); ++i) {
		if (unreserved[i].find(character) != std::string::npos) {
			return true;
		}
	}
	return false;
}

URI parseURI(const std::string &uriString) {
	URI uri;

	size_t schemeEnd = uriString.find("://");
	if (schemeEnd != std::string::npos) {
		uri.scheme = uriString.substr(0, schemeEnd);
	}

	size_t authorityStart = schemeEnd != std::string::npos ? schemeEnd + 3 : 0;
	size_t pathStart = uriString.find("/", authorityStart);
	if (pathStart != std::string::npos) {
		uri.authority = uriString.substr(authorityStart, pathStart - authorityStart);
	}
	else {
		uri.authority = uriString.substr(authorityStart);
		return uri;
	}

	size_t queryStart = uriString.find("?", pathStart);
	size_t fragmentStart = uriString.find("#", pathStart);
	if (queryStart != std::string::npos) {
		uri.path = uriString.substr(pathStart, queryStart - pathStart);
	}
	else if (fragmentStart != std::string::npos) {
		uri.path = uriString.substr(pathStart, fragmentStart - pathStart);
	}
	else {
		uri.path = uriString.substr(pathStart);
	}

	if (queryStart != std::string::npos) {
		if (fragmentStart != std::string::npos) {
			uri.query = uriString.substr(queryStart + 1, fragmentStart - queryStart - 1);
		} else {
			uri.query = uriString.substr(queryStart + 1);
		}
	}

	if (fragmentStart != std::string::npos) {
		uri.fragment = uriString.substr(fragmentStart + 1);
	}

	return uri;
}