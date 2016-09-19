/*
 * storybook.h
 *
 *  Created on: Jul 10, 2016
 *      Author: max
 */

#ifndef STORYBOOK_H_
#define STORYBOOK_H_



class storybook {
public:
	storybook(std::string workingDir);
	virtual ~storybook();
	std::string getStory();
	void assignStory(std::string story);
};

#endif /* STORYBOOK_H_ */
