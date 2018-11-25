#!/usr/bin/python2
import re
import sys

# deals with ##
def concat(content):
	pattern = r"(\".*?\"|\'.*?\')|(##)"
	# first group captures quoted strings (double or single)
	# second group captures token pasting operator (##)
	regex = re.compile(pattern, re.MULTILINE|re.DOTALL)
	def _replacer(match):
		# if the 2nd group (capturing token pasting op) is not None
		# it means that we have captured a non-quoted string
		if match.group(2) is not None:
			return ""
		else:
			return match.group(1)
	return regex.sub(_replacer, content)

# makes list of #define-d variables (ignoring whitespace)
def defList(content):
	return re.findall(r"(^#define|^\s*#define)\s+([a-zA-Z_]\w*)\s+(\d*\.?\d+|\w+)$", content, flags = re.M)


# remove lines starting with #define (ignoring whitespace)
def removedef(content):
	return re.sub(r'(^#define|^\s*#define).*\n?', '', content, flags = re.M) 

# replace all (standalone) occurences of target with repl in content
def replace(content, target, repl):
	pattern = r"(\".*?\"|\'.*?\')|(\b%s\b)" % target
	# first group captures quoted strings (double or single)
	# second group captures specified standalone target
	regex = re.compile(pattern, re.MULTILINE|re.DOTALL)
	def _replacer(match):
		# if the 2nd group (capturing target) is not None,
		# it means that we have captures a non-quoted string.
		if match.group(2) is not None:
			return repl
		else: # otherwise, we will return the entire 1st group (quoted string)
			return match.group(1)
	return regex.sub(_replacer, content)

# remove single and multi-line comments. Don't remove strings that contain "comment-like" text
def remove_comments(content):
	pattern = r"(\".*?\"|\'.*?\')|(/\*.*?\*/|//[^\r\n]*$)"
	# first group captures quoted strings (double or single)
	# second group captures comments (//single-line or /* multi-line */)
	regex = re.compile(pattern, re.MULTILINE|re.DOTALL)
	def _replacer(match):
		# if the 2nd group (capturing comments) is not None,
		# it means we have captured a non-quoted (real) comment string.
		if match.group(2) is not None:
			return "" # so we will return empty to remove the comment
		else: # otherwise, we will return the 1st group
			return match.group(1) # captured quoted-string
	return regex.sub(_replacer, content)

def main():
	# fIn = open ('test3', 'r')
	# fOut = open ('test2.txt','w')
	content = sys.stdin.read()
	
	#content = fIn.read()
	content = remove_comments(content)
	content = concat(content)

	# acquire list of define-d variables
	deflist = defList(content)
	# remove lines with #define from content
	content = removedef(content)

	# replace all define-d variables with their values
	for item in deflist:
		content = replace(content, str(item[1]), str(item[2]))

	print content
	# fOut.write(content)
	# fIn.close()
	# fOut.close()

if __name__== "__main__":
	main()