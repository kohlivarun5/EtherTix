release:
	npm run clean
	npm run build
	npm run webpack:production
	git branch -D gh-pages 
	git checkout -b gh-pages 
	git add -f build/Index.js 
	git commit -am 'Add Build'
	git push -f origin gh-pages:gh-pages  
