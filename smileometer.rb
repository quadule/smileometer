class Smiles < Array
  MINUTES = 60
  HOURS = 60*MINUTES
  RECENT = 10*MINUTES
  attr_accessor :start
  
  def initialize(*args)
    super
    @start = now
  end
  
  def now
    Time.now.to_f
  end
  
  def elapsed
    count < 1 ? 0 : now - start
  end
  
  def log
    push now
  end
  
  def in_last(seconds)
    smiles = self.class.new select { |time| (now - time) < seconds }
    smiles.start = now - seconds
    smiles
  end
  
  def per_hour
    if elapsed > RECENT
      in_last(RECENT).per_hour_smoothed
    else
      per_hour_total
    end
  end
  
  def per_hour_smoothed
    start_time = first || (now - RECENT)
    end_time = now
    range = end_time - start_time
    average_time = inject(:+) / size.to_f
    position = 1.0 / range * (average_time - start_time)
    per_hour_total * position * 2
  end
  
  def per_hour_total
    count * (HOURS / elapsed)
  end
end

Shoes.app :width => 300, :height => 200, :title => 'Smileometer' do
  @cx, @cy = width / 2, height / 2
  @border = 15
  @radius = @cx - @border
  @bottom_y = @border + @radius
  @max = 1000.0
  @sph = 0.0
  @sph_smooth = 0.0
  
  animate(4) do
    @sph = $smiles.per_hour
    @sph = 0.0 if @sph.nan? || @sph < 0
  end
  
  animate(16) do
    clear do
      @sph_smooth += (@sph - @sph_smooth) / 5
      @sph_smooth = @max if @sph_smooth > @max
      
      gauge
      indicator @sph_smooth
      readout "#{@sph.round} smiles per hour", 'left'
      readout "#{$smiles.size} smiles", 'right'
    end
  end
  
  def gauge
    background black
    stroke white
    strokewidth 3
    nofill
    
    #face
    arc @cx, @bottom_y, @radius*2, @radius*2, PI, TWO_PI
    line @border, @bottom_y, @cx+@radius, @bottom_y
    
    #dot
    strokewidth 0
    fill red
    size = 6
    oval @cx-size, @bottom_y-size, size*2, size*2
    
    ticks
  end
  
  def ticks(number=10)
    (0..number).each do |i|
      amount = @max / number * i
      indicator amount, 0.97..1.03, 1, white
    end
  end
  
  def indicator(value, range=0.0..0.9, width=3, color=red)
    strokewidth width
    stroke color
    x = @radius * Math.cos(value * PI / @max)
    y = @radius * Math.sin(value * PI / @max)
    line @cx-x*range.begin, @bottom_y-y*range.begin,
         @cx-x*range.end, @bottom_y-y*range.end
  end
  
  def readout(text, align='left')
    para text, :align => align, :stroke => white, :top => height-50, :margin => @border
  end
end

$smiles = Smiles.new
$detector = IO.popen File.join(File.dirname(__FILE__), 'Smileometer\ Camera.app/Contents/MacOS/Smileometer\ Camera')
Thread.new do
  while line = $detector.readline
    $smiles.log if line.chomp == ":)"
  end
end