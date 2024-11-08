# api를 활용한 구글맵 사용하는 것
GOOGLE_KEY = "YOUR API"
FIND_LOCATION = "부산대학교"

library(ggmap)
 
register_google(key=GOOGLE_KEY)
gc <- geocode(enc2utf8(FIND_LOCATION))

cen <- as.numeric(gc)
map <- get_googlemap(center=cen, zoom=15.5, size = c(640, 640), maptype = "roadmap", marker=gc)
ggmap(map)
 