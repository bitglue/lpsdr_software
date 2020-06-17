SUBDIRS = src
dist_doc_DATA = README

format:
	find src -name '*.cc' -or -name '*.h' | xargs clang-format -i
