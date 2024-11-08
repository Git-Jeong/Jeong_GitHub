# api를 활용한 구글맵 사용하는 것
GOOGLE_API_KEY = "YOUR API" 

names <- c("용두암", "성산일출봉", "정발폭포", "중문관광단지", "한라산1100고지", "차귀도")
addr <- c("제주시 용두암길 15", "서귀포시 성산읍 성산리", "서귀포시 동홍동 299-3", "서귀포시 중문동 2624-1", "서귀포시 색달동 산1-2", "제주시 한경면 고산리 125")

library(ggmap)

register_google(key = GOOGLE_API_KEY)
gc <- geocode(enc2utf8(addr))

# NA 값이 없는 행만 선택
gc <- na.omit(gc)
df <- data.frame(name = names, lon = gc$lon, lat = gc$lat)

# 위도, 경도 평균 계산
if (nrow(gc) > 0) { 
  cen <- c(mean(df$lon, na.rm = TRUE), mean(df$lat, na.rm = TRUE))
  
  # 지도 생성
  map <- get_googlemap(center = cen, 
                       maptype = "roadmap",
                       zoom = 10,
                       size = c(640, 640),
                       markers = gc)

ggmap(map)

gmap <<- ggmap(map)
gmap + geom_text(data=de,
                 aes(x=lon, y=lat),
                 size=5,
                 label=df$name)



ggmap(map)
