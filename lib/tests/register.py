#!/usr/bin/python -u
# -*- coding: utf-8 -*-
# /*
#  This file is part of BitPunch
#  Copyright (C) 2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
# */
TESTS = {
	"testMemLeakWithH" : {
		"suite" : "memory",
		"file" : "testMemLeaks.sh",
		"args" : "test-with-h-debug",
		"runLevel" : "regular",
	},
	"testMemLeakWoH" : {
		"suite" : "memory",
		"file" : "testMemLeaks.sh",
		"args" : "test-wo-h-debug",
		"runLevel" : "regular",
	},
	"testBuild" : {
		"suite" : "build",
		"file" : "testBuild.sh",
		"args" : "",
		"runLevel" : "express",
	}
}
