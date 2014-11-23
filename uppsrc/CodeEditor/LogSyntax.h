class LogSyntax : public EditorSyntax {
public:
	virtual void            Highlight(const wchar *s, const wchar *end, HighlightOutput& hls,
	                                  CodeEditor *editor, int line, int pos);

private:
	HighlightOutput *hout;
};