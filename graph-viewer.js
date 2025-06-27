let graphData = null;

function showStatus(message, isError = false) {
    const statusDiv = document.getElementById('status');
    statusDiv.innerHTML = message;
    statusDiv.className = 'status ' + (isError ? 'error' : 'success');
}

function updateGraphStats(data) {
    const cities = data.cities;
    const connections = data.connections || [];

    document.getElementById('graphStats').innerHTML =
        `Graph loaded: ${cities.length} cities, ${connections.length} routes`;
}

function loadFromFile() {
    const fileInput = document.getElementById('fileInput');
    const file = fileInput.files[0];

    if (!file) {
        showStatus('Please select a JSON file first.', true);
        return;
    }

    const reader = new FileReader();
    reader.onload = function (e) {
        try {
            const data = JSON.parse(e.target.result);

            if (!data.cities || !Array.isArray(data.cities)) {
                throw new Error('Invalid format: Expected "cities" array in JSON');
            }

            graphData = data;
            renderGraph(data);
            updateGraphStats(data);
            showStatus('Graph loaded successfully!');
        } catch (error) {
            showStatus('Error loading file: ' + error.message, true);
        }
    };

    reader.readAsText(file);
}

function renderGraph(data) {
    const svg = document.getElementById('graphSvg');
    svg.innerHTML = ''; // Clear existing content

    const cities = data.cities;
    const connections = data.connections || [];

    // Create a map of city names to coordinates for quick lookup
    const cityMap = {};
    cities.forEach(city => {
        cityMap[city.name] = { x: city.x, y: city.y };
    });

    // Calculate bounds for auto-sizing
    let minX = Infinity, maxX = -Infinity, minY = Infinity, maxY = -Infinity;
    cities.forEach(city => {
        minX = Math.min(minX, city.x);
        maxX = Math.max(maxX, city.x);
        minY = Math.min(minY, city.y);
        maxY = Math.max(maxY, city.y);
    });

    // Calculate available space
    const container = document.getElementById('graphContainer');
    const containerWidth = container.clientWidth || 1100;
    const containerHeight = container.clientHeight || 700;

    // Add padding and calculate scale to fit in viewport
    const padding = 50;
    const availableWidth = containerWidth - padding * 2;
    const availableHeight = containerHeight - padding * 2;

    const dataWidth = maxX - minX;
    const dataHeight = maxY - minY;

    const scaleX = availableWidth / dataWidth;
    const scaleY = availableHeight / dataHeight;
    const scale = Math.min(scaleX, scaleY, 3); // Cap at 3x for readability

    const width = dataWidth * scale + padding * 2;
    const height = dataHeight * scale + padding * 2;

    svg.setAttribute('width', width);
    svg.setAttribute('height', height);

    // Transform coordinates
    function transformX(x) {
        return (x - minX) * scale + padding;
    }

    function transformY(y) {
        return (y - minY) * scale + padding;
    }

    // Group connections by city pairs for handling multiple edges
    const connectionGroups = {};
    connections.forEach(conn => {
        const key = `${conn.city1}-${conn.city2}`;
        if (!connectionGroups[key]) {
            connectionGroups[key] = [];
        }
        connectionGroups[key].push(conn);
    });

    // Draw edges first (so they appear behind cities)
    Object.values(connectionGroups).forEach(group => {
        // Draw all edges for this city pair
        group.forEach((conn, index) => {
            const city1 = cityMap[conn.city1];
            const city2 = cityMap[conn.city2];

            if (!city1 || !city2) {
                console.warn(`City not found: ${conn.city1} or ${conn.city2}`);
                return;
            }

            const x1 = transformX(city1.x);
            const y1 = transformY(city1.y);
            const x2 = transformX(city2.x);
            const y2 = transformY(city2.y);

            const offset = (group.length > 1) ? (index - (group.length - 1) / 2) * 8 : 0;

            // Calculate perpendicular vector for offset
            const dx = x2 - x1;
            const dy = y2 - y1;
            const length = Math.sqrt(dx * dx + dy * dy);

            if (length > 0) {
                const perpX = -dy / length * offset;
                const perpY = dx / length * offset;

                const offsetX1 = x1 + perpX;
                const offsetY1 = y1 + perpY;
                const offsetX2 = x2 + perpX;
                const offsetY2 = y2 + perpY;

                // Draw edge line using proper SVG stroke properties
                // Draw gray border (thick line) - also dashed
                const borderLine = document.createElementNS('http://www.w3.org/2000/svg', 'line');
                borderLine.setAttribute('x1', offsetX1);
                borderLine.setAttribute('y1', offsetY1);
                borderLine.setAttribute('x2', offsetX2);
                borderLine.setAttribute('y2', offsetY2);
                borderLine.setAttribute('stroke', 'gray');
                borderLine.setAttribute('stroke-width', 6);
                borderLine.setAttribute('stroke-linecap', 'round');
                borderLine.setAttribute('stroke-dasharray', '10,5'); // Dashed pattern
                borderLine.setAttribute('fill', 'none');
                svg.appendChild(borderLine);

                // Draw colored fill (thinner line on top) - dashed
                const fillLine = document.createElementNS('http://www.w3.org/2000/svg', 'line');
                fillLine.setAttribute('x1', offsetX1);
                fillLine.setAttribute('y1', offsetY1);
                fillLine.setAttribute('x2', offsetX2);
                fillLine.setAttribute('y2', offsetY2);
                fillLine.setAttribute('stroke', conn.color);
                fillLine.setAttribute('stroke-width', 4);
                fillLine.setAttribute('stroke-linecap', 'round');
                fillLine.setAttribute('stroke-dasharray', '8,4'); // Dashed pattern
                fillLine.setAttribute('fill', 'none');
                fillLine.setAttribute('opacity', '0.8');
                svg.appendChild(fillLine);
            }
        });
    });

    // Draw cost circles after edges (so they appear on top)
    Object.values(connectionGroups).forEach(group => {
        // Draw one cost circle per city pair (use the first connection's cost)
        const firstConn = group[0];
        const city1 = cityMap[firstConn.city1];
        const city2 = cityMap[firstConn.city2];

        if (!city1 || !city2) {
            return;
        }

        const x1 = transformX(city1.x);
        const y1 = transformY(city1.y);
        const x2 = transformX(city2.x);
        const y2 = transformY(city2.y);

        // Calculate midpoint for cost circle
        const midX = (x1 + x2) / 2;
        const midY = (y1 + y2) / 2;

        // Draw cost circle and text (only once per city pair)
        const costCircle = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
        costCircle.setAttribute('cx', midX);
        costCircle.setAttribute('cy', midY);
        costCircle.setAttribute('r', 12);
        costCircle.setAttribute('fill', 'white');
        costCircle.setAttribute('stroke', 'black');
        costCircle.setAttribute('stroke-width', 2);
        svg.appendChild(costCircle);

        const costText = document.createElementNS('http://www.w3.org/2000/svg', 'text');
        costText.setAttribute('x', midX);
        costText.setAttribute('y', midY);
        costText.setAttribute('class', 'edge-cost');
        costText.textContent = firstConn.cost;
        svg.appendChild(costText);
    });

    // Draw cities
    cities.forEach(city => {
        const x = transformX(city.x);
        const y = transformY(city.y);

        // Split city name into words for multi-line display
        let lines = [];
        const words = city.name.split(' ');
        if (words.length === 2) {
            lines = [words[0], words[1]];
        } else {
            // Break into lines based on character length rather than word count
            let currentLine = '';
            const maxLineLength = 12; // Maximum characters per line
            words.forEach((word, index) => {
                if (currentLine === '') {
                    currentLine = word;
                } else if ((currentLine + ' ' + word).length <= maxLineLength) {
                    currentLine += ' ' + word;
                } else {
                    lines.push(currentLine);
                    currentLine = word;
                }
            });
            if (currentLine) {
                lines.push(currentLine);
            }
        }

        // Calculate circle radius based on longest line length and number of lines
        const longestLine = lines.reduce((max, line) => line.length > max.length ? line : max, '');
        const radius = Math.max(16, longestLine.length * 2.7 + lines.length * 1.75);

        // Draw city circle
        const circle = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
        circle.setAttribute('cx', x);
        circle.setAttribute('cy', y);
        circle.setAttribute('r', radius);
        circle.setAttribute('class', 'city-circle');
        svg.appendChild(circle);

        // Draw city label with multiple lines
        const lineHeight = 12;
        const totalHeight = (lines.length - 1) * lineHeight;
        const startY = y - totalHeight / 2;

        lines.forEach((line, index) => {
            const text = document.createElementNS('http://www.w3.org/2000/svg', 'text');
            text.setAttribute('x', x);
            text.setAttribute('y', startY + index * lineHeight);
            text.setAttribute('class', 'city-label');
            text.textContent = line;
            svg.appendChild(text);
        });
    });
}

// Initialize
document.addEventListener('DOMContentLoaded', function() {
    document.getElementById('fileInput').addEventListener('change', function () {
        document.getElementById('status').innerHTML = '';
    });
}); 